#pragma once

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Decl.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"

#include "include/Utility.hpp"
#include "include/metrics/FuncInfoVisitor.hpp"
#include "include/CtxVisitor.hpp"
#include "include/FunctionVisitor.hpp"

/**
 * Traverses whole TranslationUnitDecl, calculates metrics for it and then exports it.
 */
class MetricVisitor : public clang::RecursiveASTVisitor<MetricVisitor>
{
public:
    MetricVisitor();
    /**
     * Calls every metric visitor that needs to traverse only function body, for example Cyclomatic complexity. But not
     * Fan-{in, out}, those needs to know about other function as well.
     * @param decl
     * @return
     */
    bool VisitFunctionDecl(clang::FunctionDecl *decl);
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl);

    void CalcMetrics(clang::ASTContext *ctx);
    std::ostream& ExportMetrics(std::ostream &os);
    std::ostream& ExportXMLMetrics(std::ostream &os);
    bool TraverseDecl(clang::Decl *decl);
protected:
    std::map<std::string, std::vector<std::unique_ptr<FunctionVisitor> > > functions;
    std::set<std::string> classes;

    std::vector<std::unique_ptr<CtxVisitor> > ctx_vis_cl;
    std::vector<std::unique_ptr<CtxVisitor> > ctx_vis_fn;

    clang::ASTContext *context;
};
