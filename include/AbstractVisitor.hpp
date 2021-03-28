//
// Created by filip on 3/22/21.
//

#ifndef METRICS_ABSTRACTVISITOR_HPP
#define METRICS_ABSTRACTVISITOR_HPP

#include <ostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "include/Metric.hpp"


class AbstractVisitor
{
public:
    AbstractVisitor(clang::ASTContext *ctx) : context(ctx) {}
    virtual ~AbstractVisitor() = default;
    virtual std::ostream& Export(std::ostream &os) const
    {
        for(const auto & x : metrics)
            os << x.name << " - " << x.val << "\n";
        return os;
    };
    virtual void CalcMetrics(clang::Decl *decl) = 0;
    std::vector<Metric> GetMetrics() const { return metrics; }
protected:
    std::vector<Metric> metrics;
    clang::ASTContext *context;
};


#endif //METRICS_ABSTRACTVISITOR_HPP
