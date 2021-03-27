
#ifndef METRICS_METRICVISITOR_HPP
#define METRICS_METRICVISITOR_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"

#include "include/Metric.hpp"
#include "include/FuncInfoVisitor.hpp"
#include "include/CyclomaticVisitor.hpp"



class MetricVisitor : public clang::RecursiveASTVisitor<MetricVisitor>
{
public:
    MetricVisitor(clang::ASTContext *context) : context(context) {}
    bool VisitFunctionDecl(clang::FunctionDecl *decl);

protected:
    clang::ASTContext *context;
};


#endif //METRICS_METRICVISITOR_HPP
