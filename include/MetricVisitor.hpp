
#ifndef METRICS_METRICVISITOR_HPP
#define METRICS_METRICVISITOR_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Decl.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"

#include "include/Metric.hpp"
#include "include/metrics/FuncInfoVisitor.hpp"
#include "include/metrics/CyclomaticVisitor.hpp"



class MetricVisitor : public clang::RecursiveASTVisitor<MetricVisitor>
{
public:
    MetricVisitor(clang::ASTContext *context) : context(context) {}
    /**
     * Calls every metric visitor that needs to traverse only function body, for example Cyclomatic complexity. But not
     * Fan-{in, out}, those needs to know about other function as well.
     * @param decl
     * @return
     */
    bool VisitFunctionDecl(clang::FunctionDecl *decl);

    /**
     * Calls every metric visitor that needs to traverse whole code, for example Fan-{in, out} or metrics
     * that investigate class relationships.
     * @param decl
     * @return
     */
    bool VisitTranslationUnitDecl(clang::TranslationUnitDecl *decl);

protected:
    clang::ASTContext *context;
};


#endif //METRICS_METRICVISITOR_HPP
