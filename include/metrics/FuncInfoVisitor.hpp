#pragma once

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

#include "include/Utility.hpp"
#include "include/MetricVisitor.hpp"
#include "include/FunctionVisitor.hpp"

class FuncInfoVisitor : public FunctionVisitor
{
    /* Statements that contains other statements */
    static const std::array<clang::Stmt::StmtClass, 11> compoundStatements;

    struct FunctionInfo
    {
        int statements{0};
        int statements_tbd{0};
        int depth{0};
        int physical_loc{0};
    };
public:
    FuncInfoVisitor(clang::ASTContext *ctx) : FunctionVisitor(ctx) {}
    virtual ~FuncInfoVisitor() = default;
    virtual void CalcMetrics(clang::FunctionDecl *decl) override;
    FunctionInfo GetResult() const { return f; }
    virtual std::ostream &Export(std::ostream &os) const override;
    virtual std::ostream &ExportXML(std::ostream &os) const override;
private:
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
     * @return - FunctionInfo struct containing maximum depth of this statement and number of statements in this statement.
     *
     * If statement needs to be done separately, because the 'else' branch is child of the 'if' statement(its not on the same level),
     * and depth would not be calculated properly.
     */
    std::pair<int, int> HandleIfStatement(const clang::IfStmt *stmt, int depth);

    std::pair<int, int> HandleOtherCompounds(const clang::Stmt *body, int depth);

    std::pair<int, int> StmtCount(const clang::Stmt *body, int depth = 0);

    /**
     * Stores current metric values.
     */
    FunctionInfo f;
};
