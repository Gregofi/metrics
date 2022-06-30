#pragma once

#include <map>
#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "include/MetricVisitor.hpp"
#include "include/Utility.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/Logging.hpp"

class TokenCounter : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    explicit TokenCounter(bool isOperator = false) : isOperator(isOperator) {}
    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
    int getCount() const { return count; }
    int getUniqueCount() const { return seen_tokens_stmt.size() + seen_tokens_decl.size() + seen_tokens_type.size(); }
private:
    bool isOperator;
    int count = 0;
    std::set<clang::Stmt::StmtClass> seen_tokens_stmt;
    std::set<clang::Decl::Kind> seen_tokens_decl;
    std::set<clang::Type::TypeClass> seen_tokens_type;
};

/**
 * Counts the Halstead metrics.
 *
 * Rules for determining if token is operand or operator:
 * Operators:
 *  All control flow statements - for, while, do, switch, case, default, goto, break, continue, return, try, catch
 *  All operators - Unary operators, Binary operators and Conditional operator (Also all overloaded operators)
 *  Assignments:
 *      There is a special case with assignments
 *          int a;
 *      contains one operator, 'int', but
 *          int a = 1;
 *      also contains one operator, because it can be written as
 *          int a{1};
 *      so we won't count initializing with value as special operator(even though it seems as binary operator).
 *
 * Operands:
 */
class HalsteadVisitor : public FunctionVisitor
{
public:
    explicit HalsteadVisitor(clang::ASTContext *ctx);

    virtual void CalcMetrics(clang::FunctionDecl *decl) override;
    int GetOperatorCount() const {return operators;}
    int GetOperandCount() const {return operands;}
    int GetUniqueOperandCount() const {return unique_operands;}
    int GetUniqueOperatorCount() const {return unique_operators;}

    virtual std::ostream &Export(std::ostream &os) const override;
    virtual std::ostream &ExportXML(std::ostream &os) const override;
protected:
    int operators;
    int operands;
    int unique_operators;
    int unique_operands;

    static const std::vector<clang::ast_matchers::StatementMatcher>   operators_stmt;
    static const std::vector<clang::ast_matchers::DeclarationMatcher> operators_decl;

    static const std::vector<clang::ast_matchers::StatementMatcher> operands_stmt;
    static const std::vector<clang::ast_matchers::DeclarationMatcher> operands_decl;
};
