
#ifndef METRICS_METRICVISITOR_HPP
#define METRICS_METRICVISITOR_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "Metric.hpp"

template <class T>
class MetricVisitor : public clang::RecursiveASTVisitor<T>
{
public:
    MetricVisitor(std::map<int64_t, Function> &functions) : functions(functions) {}

protected:
    std::map<int64_t, Function> &functions;
};

#endif //METRICS_METRICVISITOR_HPP
