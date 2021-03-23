
#ifndef METRICS_METRIC_HPP
#define METRICS_METRIC_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

struct Metric
{
    std::string name;
    int val;
};

#endif //METRICS_METRIC_HPP
