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
    bool VisitDecl(Decl *decl)
    {

    }

    bool VisitType(Type *type)
    {

    }

    bool VisitStmt(Stmt *stmt)
    {

    }

    virtual void calcMetric(Decl *decl) override
    {
        this->TraverseDecl(decl);
    }

    virtual std::ostream& Export(std::ostream &os) override
    {

    }
protected:
    int operator_count = 0;
    int operand_count = 0;
    std::set<int> seen_operators;
    std::set<int> seen_operands;

    static constexpr std::array<clang::ast_matchers>

    Metric result;
    MatchFinder finder;
    MatchFinder::MatchCallback *callback;
};


#endif //METRICS_HALSTEADVISITOR_HPP
