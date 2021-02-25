
#ifndef METRICS_FUNCINFO_HPP
#define METRICS_FUNCINFO_HPP


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

#include "Metric.hpp"
#include "MetricVisitor.hpp"

using namespace clang;
using clang::Stmt;

class FuncInfoVisitor : public clang::RecursiveASTVisitor<FuncInfoVisitor>
{
    /* Statements that contains other statements */
    static const std::array<Stmt::StmtClass, 10> compoundStatements;
    struct Result
    {
        int statements;
        int depth;
    };

public:
    FuncInfoVisitor(ASTContext *ctx) : context(ctx) {}

    /**
     * Calculates number of lines for given function body.
     * @param decl
     * @return
     *
     * This calculates real number of lines, empty lines and comments are also counted.
     */
    int CalcLength(FunctionDecl *decl);

    /**
     * Handles calculation of depth and statements for if statement.
     * @param stmt - If statement
     * @param depth - current depth
     * @return - Result struct containing maximum depth of this statement and number of statements in this statement.
     *
     * If statement needs to be done separately, because the 'else' branch is child of the 'if' statement(its not on the same level),
     * and depth would not be calculated properly.
     */
    Result HandleIfStatement(const Stmt *stmt, int depth);

    Result HandleOtherCompounds(const Stmt *body, int depth);

    Result StmtCount(const Stmt *body, int depth = 0);

    /**
     * Calculates range of function declaration.
     * @param decl
     * @return
     */
    bool VisitFunctionDecl(FunctionDecl *decl);

    bool VisitCXXRecordDecl(CXXRecordDecl *decl)
    {
        clang::SourceManager &sm(context->getSourceManager());
        /* Don't calc if source code is not in main file. */
        if(!sm.isInMainFile(decl->getLocation())){
            return true;
        }

        decl->forallBases([](const CXXRecordDecl *decl){
            llvm::outs() << "--+==" << decl->getNameAsString() << "\n";
            return true;
        });
        return true;
    }

    std::vector<Metric> calcMetric(Decl *decl)
    {
        this->TraverseDecl(decl);
        return res;
    }

private:
    std::vector<Metric> res;
    ASTContext *context;
};

#endif //METRICS_FUNCINFO_HPP
