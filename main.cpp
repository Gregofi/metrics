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

using namespace clang;
using clang::Stmt;

struct Function
{
    std::string name;
    int length;
    int stmtCnt;
    int maxDepth;
};

class FunctionInfoVisitor : public RecursiveASTVisitor<FunctionInfoVisitor>
{
    /* Statements that contains other statements */
    static const std::array<Stmt::StmtClass, 10> compoundStatements;
    struct Result
    {
        int statements;
        int depth;
    };

public:
    explicit FunctionInfoVisitor(ASTContext *context)
            : context(context) {}

    /**
     * Calculates number of lines for given function body.
     * @param decl
     * @return
     *
     * This calculates real number of lines, empty lines and comments are also counted.
     */
    int CalcLength(FunctionDecl *decl)
    {
        clang::SourceManager &sm(context->getSourceManager());
        if(decl->hasBody())
            return sm.getSpellingLineNumber(decl->getBody()->getEndLoc())
                       - sm.getSpellingLineNumber(decl->getBody()->getBeginLoc());
        return 0;
    }

    /**
     * Handles calculation of depth and statements for if statement.
     * @param stmt - If statement
     * @param depth - current depth
     * @return - Result struct containing maximum depth of this statement and number of statements in this statement.
     *
     * If statement needs to be done separately, because the 'else' branch is child of the 'if' statement,
     * and depth would not be calculated properly.
     */
    Result HandleIfStatement(const Stmt *stmt, int depth)
    {
        using clang::Stmt;
        assert(stmt->getStmtClass() == Stmt::IfStmtClass);

        int cnt = 0;
        auto it = stmt->child_begin();
        /* For statement has three potential childs: condition, body and else.
         * We will skip the condition. */
        std::advance(it, 1);
        printf("%s\n", it->getStmtClassName());
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

    Result HandleOtherCompounds(const Stmt *body, int depth)
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

    Result StmtCount(const Stmt *body, int depth = 0)
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

    /**
     * Calculates range of function declaration.
     * @param decl
     * @return
     */
    bool VisitFunctionDecl(FunctionDecl *decl)
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
            funcs.insert({decl->getID(), {decl->getQualifiedNameAsString(), CalcLength(decl),
                                                    res.statements, res.depth}});
        }
        return true;
    }

    std::map<int, Function> GetFunctions()
    {
        return std::move(funcs);
    }

private:
    ASTContext *context;
    std::map<int, Function> funcs;
};

const std::array<Stmt::StmtClass, 10> FunctionInfoVisitor::compoundStatements  ({
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

/* ======================================================================================= */

class CyclomaticVisitor : public RecursiveASTVisitor<CyclomaticVisitor>
{
public:
    explicit CyclomaticVisitor(ASTContext *context) : context(context) {}
    bool VisitFunctionDecl(FunctionDecl *decl)
    {
        llvm::outs() << decl->getQualifiedNameAsString() << "\n";
        return true;
    }
private:
    ASTContext *context;
};

/* ======================================================================================= */

class FunctionInfoConsumer : public clang::ASTConsumer
{
public:
    explicit FunctionInfoConsumer(ASTContext *context) : visitorFunc(context), visitorCycl(context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &context)
    {
        visitorFunc.TraverseDecl(context.getTranslationUnitDecl());
        visitorCycl.TraverseDecl(context.getTranslationUnitDecl());
        for(const auto & x : visitorFunc.GetFunctions())
        {
            auto func = x.second;
            llvm::outs() << "name: " << func.name << "\n"
                         << "length: " << func.length << "\n"
                         << "stmtcnt:" << func.stmtCnt << "\n"
                         << "depth:" << func.maxDepth << "\n----------------\n";
        }
    }
private:
    FunctionInfoVisitor visitorFunc;
    CyclomaticVisitor visitorCycl;
};

class FunctionInfoAction : public clang::ASTFrontendAction
{
public:
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
    {
        return std::unique_ptr<FunctionInfoConsumer>(new FunctionInfoConsumer(&Compiler.getASTContext()));
    }
};


int main(int argc, char **argv) {
    std::ifstream fs(argv[1]);
    std::string s, l;
    while(std::getline(fs, l))
        s += l + '\n';
    if (argc > 1) {
        clang::tooling::runToolOnCode(std::make_unique<FunctionInfoAction>(), s, argv[1]);
    }
}
