#include <vector>
#include <memory>
#include <iostream>

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"

#include "include/MetricVisitor.hpp"
#include "include/metrics/CyclomaticVisitor.hpp"
#include "include/metrics/FuncInfoVisitor.hpp"
#include "include/metrics/HalsteadVisitor.hpp"
#include "include/metrics/NPathVisitor.hpp"
#include "include/metrics/ClassOverviewVisitor.hpp"
#include "include/metrics/FansVisitor.hpp"

#include "include/FunctionVisitor.hpp"


bool MetricVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
    clang::SourceManager &sm(context->getSourceManager());

    /* Don't calc if source code is not in main file. */
    if(!sm.isInMainFile(decl->getLocation())
        || !decl->isThisDeclarationADefinition()
        || !decl->hasBody()){
        return true;
    }

    std::vector<std::unique_ptr<FunctionVisitor> > visitors;

    visitors.emplace_back(std::make_unique<FuncInfoVisitor>(context));
    visitors.emplace_back(std::make_unique<CyclomaticVisitor>(context));
    visitors.emplace_back(std::make_unique<HalsteadVisitor>(context));
    visitors.emplace_back(std::make_unique<NPathVisitor>(context));

    for(const auto & x : visitors)
        x->CalcMetrics(decl);

    functions[decl->getID()] = {decl->getQualifiedNameAsString(), std::move(visitors)};

    return true;
}

bool MetricVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *decl)
{
    clang::SourceManager &sm(context->getSourceManager());

    if(decl->isLambda() || !decl->hasDefinition() || !sm.isInMainFile(decl->getLocation()) || decl->isUnion())
        return true;
    classes.emplace_back(decl->getQualifiedNameAsString(), decl->getID());
    return true;
}

void MetricVisitor::CalcMetrics(clang::TranslationUnitDecl *decl)
{
    TraverseTranslationUnitDecl(decl);
    for(auto &x :ctx_vis_cl)
        x->CalcMetrics(decl);
    for(auto &x :ctx_vis_fn)
        x->CalcMetrics(decl);
}

std::ostream& MetricVisitor::ExportMetrics(std::ostream &os)
{
    bool first = true;
    os << "Function metrics:\n";
    for(const auto &f : functions)
    {
        if(!first)
            os << "--------------------------------\n";
        os << f.second.first << "\n";
        for(const auto &m: f.second.second)
            m->Export(os);
        for(const auto &m: ctx_vis_fn)
            m->Export(f.first, os);
        first = false;
    }
    os << "OOP metrics:\n";
    for(const auto &cl : classes)
    {

        if(!first)
            os << "--------------------------------\n";
        for(const auto &cvis: ctx_vis_cl)
        {
            cvis->Export(cl.second, os);
        }
        first = false;
    }
    return os;
}

MetricVisitor::MetricVisitor(clang::ASTContext *context) : context(context)
{
    ctx_vis_cl.emplace_back(std::make_unique<ClassOverviewVisitor>(context));
    ctx_vis_fn.emplace_back(std::make_unique<FansVisitor>(context));
}
