#include <fstream>
#include <set>
#include <array>
#include <algorithm>
#include <cassert>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "FuncInfoVisitor.hpp"


const std::array<Stmt::StmtClass, 10> FuncInfoVisitor::compoundStatements  ({
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
});


int FuncInfoVisitor::CalcLength(FunctionDecl *decl)
{
    clang::SourceManager &sm(context->getSourceManager());
    if(decl->hasBody())
        return sm.getSpellingLineNumber(decl->getBody()->getEndLoc())
               - sm.getSpellingLineNumber(decl->getBody()->getBeginLoc());
    return 0;
}

FuncInfoVisitor::Result FuncInfoVisitor::HandleIfStatement(const Stmt *stmt, int depth)
{
    using clang::Stmt;
    assert(stmt->getStmtClass() == Stmt::IfStmtClass);

    int cnt = 0;
    auto it = stmt->child_begin();
    /* For statement has three potential childs: condition, body and else.
     * We will skip the condition. */
    std::advance(it, 1);
    /* This is the 'if' body */
    auto res_if = StmtCount(*it, depth);
    std::advance(it, 1);
    /* This is 'else' branch, for our purposes, this is NOT counted as depth increase. */
    Result res_else{0, 0};
    if(it != stmt->child_end())
    {
        bool is_else = it->getStmtClass() != Stmt::IfStmtClass;
        res_else = StmtCount(*it, depth);
        if(is_else)
            res_else.statements += 1;
    }

    cnt += res_else.statements + res_if.statements;
    depth = std::max(res_if.depth, res_else.depth);
    return {cnt, depth};
}

FuncInfoVisitor::Result FuncInfoVisitor::HandleOtherCompounds(const Stmt *body, int depth)
{
    auto it = body->children().begin();
    auto stmtClass = body->getStmtClass();
    /* First children of these statements are some sort of conditions or initializations, skip that */
    if(stmtClass == Stmt::WhileStmtClass || stmtClass == Stmt::SwitchStmtClass
       || stmtClass == Stmt::CXXForRangeStmtClass || stmtClass == Stmt::CaseStmtClass)
        std::advance(it, 1);

    /* Here, for cycle contains four other statements before body, also just
     * skip them */
    if(stmtClass == Stmt::ForStmtClass)
        std::advance(it, 4);

    Result res = {0, depth};
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
                          || stmtClass == Stmt::CaseStmtClass));
        auto tmp_res = StmtCount(*it, depth + advance);
        res.statements += tmp_res.statements;
        res.depth = std::max(res.depth, tmp_res.depth);
    }
    return res;
}

FuncInfoVisitor::Result FuncInfoVisitor::StmtCount(const Stmt *body, int depth)
{
    /* Some statements can be null, for example for(;;) has four null statements
     * as its children. */
    if(!body)
        return {0, 0};

    using clang::Stmt;

    auto stmtClass = body->getStmtClass();
    bool is_compound = std::find(compoundStatements.begin(), compoundStatements.end(), stmtClass)
                       != compoundStatements.end();

    /* Do not count Compound Statement as statement */
    Result res = {!(stmtClass == Stmt::CompoundStmtClass), depth};

    if(is_compound)
    {
        Result tmp_res;

        /* Because else branch is child of if statement, we need to deal with it
         * separately. */
        if(stmtClass == Stmt::IfStmtClass)
            tmp_res = HandleIfStatement(body, depth);
        else
            tmp_res = HandleOtherCompounds(body, depth);

        res.statements += tmp_res.statements;
        res.depth = std::max(res.depth, tmp_res.depth);
    }
    return res;
}

bool FuncInfoVisitor::VisitFunctionDecl(FunctionDecl *decl)
{
    clang::SourceManager &sm(context->getSourceManager());
    /* Don't calc if source code is not in main file. */
    if(!sm.isInMainFile(decl->getLocation())){
        return true;
    }
    decl->dump();
    /* Only calculate length if its also a definition */
    if(decl->isThisDeclarationADefinition())
    {
        auto res = StmtCount(decl->getBody());
        Function func(decl->getQualifiedNameAsString(), {
            {"Physical Lines of code", CalcLength(decl)},
            {"Number of statements", res.statements},
            {"Maximum depth", res.depth},
        });
        funcs.insert({decl->getID(), func});
    }
    return true;
}

std::map<int64_t, Function> FuncInfoVisitor::GetFunctions()
{
    return std::move(funcs);
}