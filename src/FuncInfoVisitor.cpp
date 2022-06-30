#include <fstream>
#include <set>
#include <array>
#include <algorithm>
#include <cassert>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtCXX.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "include/metrics/FuncInfoVisitor.hpp"

using namespace clang;

const std::array<Stmt::StmtClass, 11> FuncInfoVisitor::compoundStatements  ({
    Stmt::SwitchStmtClass,
    Stmt::IfStmtClass,
    Stmt::ForStmtClass,
    Stmt::CompoundStmtClass,
    Stmt::WhileStmtClass,
    Stmt::CXXForRangeStmtClass,
    Stmt::CXXTryStmtClass,
    Stmt::CXXCatchStmtClass,
    Stmt::CoroutineBodyStmtClass,
    Stmt::CaseStmtClass,
    Stmt::DefaultStmtClass,
});


int FuncInfoVisitor::CalcLength(FunctionDecl *decl)
{
    clang::SourceManager &sm(context->getSourceManager());
    if(decl->hasBody())
        return sm.getSpellingLineNumber(decl->getBody()->getEndLoc())
               - sm.getSpellingLineNumber(decl->getBody()->getBeginLoc());
    return 0;
}

std::pair<int, int> FuncInfoVisitor::HandleIfStatement(const IfStmt *stmt, int depth)
{
    using clang::Stmt;

    int cnt = 0;
    /* This is the 'if' body */
    std::pair<int, int> res_if = StmtCount(stmt->getThen(), depth + (stmt->getThen()->getStmtClass() != Stmt::CompoundStmtClass));
    /* This is 'else' branch, for our purposes, this is NOT counted as depth increase. */
    std::pair<int, int> res_else = std::make_pair(0, 0);
    if(stmt->getElse())
        res_else = StmtCount(stmt->getElse(), depth + (stmt->getElse()->getStmtClass() != Stmt::CompoundStmtClass));

    cnt += res_else.first + res_if.first;
    depth = std::max(res_if.second, res_else.second);
    return {cnt + (stmt->getElse() != nullptr), depth};
}

std::pair<int, int> FuncInfoVisitor::HandleOtherCompounds(const Stmt *body, int depth)
{
    auto it = body->children().begin();
    auto stmtClass = body->getStmtClass();
    /* First children of these statements are some sort of conditions or initializations, skip that */
    if(stmtClass == Stmt::WhileStmtClass || stmtClass == Stmt::SwitchStmtClass
       || stmtClass == Stmt::CaseStmtClass)
        std::advance(it, 1);

    /* Here, for cycle contains four other statements before body, also just
     * skip them */
    if(stmtClass == Stmt::ForStmtClass)
        std::advance(it, 4);

    std::pair<int, int> res = {0, depth};
    for(;it != body->child_end(); ++it)
    {
        if(!*it)
            continue;
        /* This is true if child is compound statement and parent is for example 'for' statement,
         * this would cause to add depth twice */
        bool advance = !(it->getStmtClass() == Stmt::CompoundStmtClass &&
                         (stmtClass == Stmt::WhileStmtClass || stmtClass == Stmt::CXXTryStmtClass
                          || stmtClass == Stmt::ForStmtClass || stmtClass == Stmt::SwitchStmtClass
                          || stmtClass == Stmt::CXXForRangeStmtClass || stmtClass == Stmt::CXXCatchStmtClass
                          || stmtClass == Stmt::CaseStmtClass || stmtClass == Stmt::DefaultStmtClass));
        std::pair<int, int> tmp_res = StmtCount(*it, depth + advance);
        res.first += tmp_res.first;
        res.second = std::max(res.second, tmp_res.second);
    }
    return res;
}

std::pair<int, int> FuncInfoVisitor::StmtCount(const Stmt *body, int depth)
{
    /* Some statements can be null, for example for(;;) has four null statements
     * as its children. */
    if(!body)
        return {0, 0};

    using clang::Stmt;

    auto stmtClass = body->getStmtClass();
    bool is_compound =
            std::find(compoundStatements.begin(), compoundStatements.end(), stmtClass)!= compoundStatements.end();

    /* Do not count Compound Statement as statement */
    std::pair<int, int> res = std::make_pair(!(stmtClass == Stmt::CompoundStmtClass), depth);

    if(is_compound)
    {
        std::pair<int, int> tmp_res;

        /* Because else branch is child of if statement(ie. it would increase depth), we need to deal with it
         * separately. */
        if(stmtClass == Stmt::IfStmtClass)
            tmp_res = HandleIfStatement(llvm::dyn_cast<IfStmt>(body), depth);
        else if(stmtClass == Stmt::CXXForRangeStmtClass)
        {
            const CXXForRangeStmt *forRange = llvm::dyn_cast<CXXForRangeStmt>(body);
            tmp_res = StmtCount(forRange->getBody(), depth + (forRange->getBody()->getStmtClass() != Stmt::CompoundStmtClass));
        }
        else if(stmtClass == Stmt::CXXCatchStmtClass)
        {
            const CXXCatchStmt *catchStmt = llvm::dyn_cast<CXXCatchStmt>(body);
            tmp_res = StmtCount(llvm::dyn_cast<CompoundStmt>(catchStmt->getHandlerBlock()), depth);
            /* Catch is child of try, so decrease the depth. */
            tmp_res.second -= 1;
        }
        else
            tmp_res = HandleOtherCompounds(body, depth);

        res.first += tmp_res.first;
        res.second = std::max(res.second, tmp_res.second);
    }
    return res;
}

std::ostream &FuncInfoVisitor::Export(std::ostream &os) const
{
    os << "Lines of code: " << f.physical_loc << "\n";
    os << "Number of statements: " << f.statements << "\n";
    os << "Maximum depth: " << f.depth << "\n";
    return os;
}

std::ostream &FuncInfoVisitor::ExportXML(std::ostream &os) const
{
    os << Tag("lines", f.physical_loc) << Tag("statements", f.statements) << Tag("depth", f.depth);
    return os;
}

void FuncInfoVisitor::CalcMetrics(clang::FunctionDecl *decl)
{
    /* Only calculate length if its also a definition */
    if(decl->isThisDeclarationADefinition())
    {
        auto res = StmtCount(decl->getBody());
        f.statements = res.first;
        f.depth = res.second;
        f.physical_loc = CalcLength(decl);
    }
}
