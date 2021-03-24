//
// Created by filip on 3/22/21.
//

#ifndef METRICS_HALSTEADVISITOR_HPP
#define METRICS_HALSTEADVISITOR_HPP


#include <map>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "MetricVisitor.hpp"
#include "Metric.hpp"

class HalsteadVisitor : public AbstractVisitor, public clang::RecursiveASTVisitor<HalsteadVisitor>
{
public:
    bool VisitDecl(clang::Decl *decl)
    {

    }

    bool VisitType(clang::Type *type)
    {

    }

    bool VisitStmt(clang::Stmt *stmt)
    {

    }

    virtual void CalcMetrics(clang::Decl *decl) override
    {
        this->TraverseDecl(decl);
    }

protected:
    int operator_count = 0;
    int operand_count = 0;
    std::set<int> seen_operators;
    std::set<int> seen_operands;

//    static constexpr std::vector<clang::ast_matchers::StatementMatcher> matchers;

    Metric result;
    clang::ast_matchers::MatchFinder finder;
    clang::ast_matchers::MatchFinder::MatchCallback *callback;
};


#endif //METRICS_HALSTEADVISITOR_HPP
