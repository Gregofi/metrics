
#ifndef METRICS_FUNCINFO_HPP
#define METRICS_FUNCINFO_HPP


#include <fstream>
#include <set>
#include <array>
#include <algorithm>
#include <cassert>
#include <ostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "include/Metric.hpp"
#include "include/MetricVisitor.hpp"
#include "include/AbstractVisitor.hpp"

class FuncInfoVisitor : public AbstractVisitor, public clang::RecursiveASTVisitor<FuncInfoVisitor>
{
    /* Statements that contains other statements */
    static const std::array<clang::Stmt::StmtClass, 10> compoundStatements;
    struct Result
    {
        int statements;
        int depth;
    };


public:
    FuncInfoVisitor(clang::ASTContext *ctx) : AbstractVisitor(ctx) {}
    virtual ~FuncInfoVisitor() = default;
    /**
     * Calculates number of lines for given function body.
     * @param decl
     * @return
     *
     * This calculates real number of lines, empty lines and comments are also counted.
     */
    int CalcLength(clang::FunctionDecl *decl);

    /**
     * Handles calculation of depth and statements for if statement.
     * @param stmt - If statement
     * @param depth - current depth
     * @return - Result struct containing maximum depth of this statement and number of statements in this statement.
     *
     * If statement needs to be done separately, because the 'else' branch is child of the 'if' statement(its not on the same level),
     * and depth would not be calculated properly.
     */
    Result HandleIfStatement(const clang::IfStmt *stmt, int depth);

    Result HandleOtherCompounds(const clang::Stmt *body, int depth);

    Result StmtCount(const clang::Stmt *body, int depth = 0);

    /**
     * Calculates range of function declaration.
     * @param decl
     * @return
     */
    bool VisitFunctionDecl(clang::FunctionDecl *decl);

    virtual void CalcMetrics(clang::Decl *decl) override
    {
        this->TraverseDecl(decl);
    }
};

#endif //METRICS_FUNCINFO_HPP
