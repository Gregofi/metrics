//
// Created by filip on 3/22/21.
//

#ifndef METRICS_HALSTEADVISITOR_HPP
#define METRICS_HALSTEADVISITOR_HPP


#include <map>
#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "MetricVisitor.hpp"
#include "Metric.hpp"
#include "ASTMatcherVisitor.hpp"

class TokenCounter : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    TokenCounter(bool isOperator = false) : isOperator(isOperator) {}
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override
    {
        count += 1;
        if(const auto *s = Result.Nodes.getNodeAs<clang::Stmt>("stmt"))
        {
            seen_tokens_stmt.emplace(s->getStmtClass());
        }
        if(const Type *t = Result.Nodes.getNodeAs<clang::Type>("type"))
        {
            seen_tokens_type.emplace(t->getTypeClass());
        }
        if(const Decl *d = Result.Nodes.getNodeAs<clang::Decl>("decl"))
        {
            seen_tokens_decl.emplace(d->getKind());
            if(const auto *vardecl = llvm::dyn_cast<clang::VarDecl>(d); isOperator && vardecl)
            {
                count += vardecl->hasInit();
            }
        }
    }
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
class HalsteadVisitor : public AbstractVisitor, public clang::RecursiveASTVisitor<HalsteadVisitor>
{
public:
    explicit HalsteadVisitor(clang::ASTContext *ctx);

    virtual void CalcMetrics(clang::Decl *decl) override
    {
        matcher.TraverseDecl(decl);
        metrics.push_back({"Operator count", tk_operators.getCount()});
        metrics.push_back({"Operand count", tk_operand.getCount()});
    }

protected:
    TokenCounter tk_operators;
    TokenCounter tk_operand;
    ASTMatcherVisitor matcher;
    std::set<clang::Stmt> seen_operators;
    std::set<int> seen_operands;

    static const std::vector<clang::ast_matchers::StatementMatcher>   operators_stmt;
    static const std::vector<clang::ast_matchers::DeclarationMatcher> operators_decl;
    static const std::vector<clang::ast_matchers::TypeMatcher>        operators_type;

    static const std::vector<clang::ast_matchers::StatementMatcher> operands_stmt;
    static const std::vector<clang::ast_matchers::DeclarationMatcher> operands_decl;
};


#endif //METRICS_HALSTEADVISITOR_HPP
