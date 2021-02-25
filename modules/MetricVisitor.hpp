
#ifndef METRICS_METRICVISITOR_HPP
#define METRICS_METRICVISITOR_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"

#include "Metric.hpp"
#include "FuncInfoVisitor.hpp"
#include "CyclomaticVisitor.hpp"



class MetricVisitor : public clang::RecursiveASTVisitor<MetricVisitor>
{
public:
    MetricVisitor(clang::ASTContext *context) : context(context) {}
    bool VisitFunctionDecl(clang::FunctionDecl *decl);

protected:
    std::map<int64_t, Function> funcs;
    clang::ASTContext *context;
};


#endif //METRICS_METRICVISITOR_HPP
