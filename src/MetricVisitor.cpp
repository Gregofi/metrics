#include <iostream>
#include <memory>
#include <vector>

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"

#include "include/FunctionVisitor.hpp"
#include "include/MetricVisitor.hpp"
#include "include/Utility.hpp"
#include "include/metrics/ClassOverviewVisitor.hpp"
#include "include/metrics/CyclomaticVisitor.hpp"
#include "include/metrics/FansVisitor.hpp"
#include "include/metrics/FuncInfoVisitor.hpp"
#include "include/metrics/HalsteadVisitor.hpp"
#include "include/metrics/NPathVisitor.hpp"

bool MetricVisitor::VisitFunctionDecl(clang::FunctionDecl* decl)
{
    clang::SourceManager& sm(context->getSourceManager());

    /* Don't calc if source code is not in main file. */
    if (!decl || isInSystemHeader(context->getSourceManager(), decl->getLocation())
        || !decl->isThisDeclarationADefinition()
        || functions.count(GetFunctionHead(decl))
        || !decl->hasBody()) {
        return true;
    }

    std::vector<std::unique_ptr<FunctionVisitor>> visitors;

    visitors.emplace_back(std::make_unique<FuncInfoVisitor>(context));
    visitors.emplace_back(std::make_unique<CyclomaticVisitor>(context));
    visitors.emplace_back(std::make_unique<NPathVisitor>(context));

    for (const auto& x : visitors) {
        x->CalcMetrics(decl);
    }
    functions[GetFunctionHead(decl)] = std::move(visitors);

    return true;
}

bool MetricVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* decl)
{
    if (!decl || decl->isLambda() || !decl->isThisDeclarationADefinition() || isInSystemHeader(context->getSourceManager(), decl->getLocation())
        || (!decl->isStruct() && !decl->isClass()) || classes.count(decl->getQualifiedNameAsString()))
        return true;
    classes.insert(decl->getQualifiedNameAsString());
    return true;
}

void MetricVisitor::CalcMetrics(clang::ASTContext* ctx)
{
    context = ctx;
    TraverseTranslationUnitDecl(ctx->getTranslationUnitDecl());
    for (auto& x : ctx_vis_cl)
        x->CalcMetrics(ctx);
    LOG("First pass");
    for (auto& x : ctx_vis_fn)
        x->CalcMetrics(ctx);
    context = nullptr;
}

std::ostream& MetricVisitor::ExportMetrics(std::ostream& os)
{
    bool first = true;
    os << "Function metrics:\n";
    for (const auto& f : functions) {
        if (!first)
            os << "--------------------------------\n";
        os << f.first << "\n";
        for (const auto& m : f.second)
            m->Export(os);
        for (const auto& m : ctx_vis_fn)
            m->Export(f.first, os);
        first = false;
    }
    os << "OOP metrics:\n";
    for (const auto& cl : classes) {
        if (!first)
            os << "--------------------------------\n";
        os << cl << "\n";
        for (const auto& cvis : ctx_vis_cl) {
            cvis->Export(cl, os);
        }
        first = false;
    }
    return os;
}

MetricVisitor::MetricVisitor()
{
    ctx_vis_cl.emplace_back(std::make_unique<ClassOverviewVisitor>());
    ctx_vis_fn.emplace_back(std::make_unique<FansVisitor>());
}

std::ostream& MetricVisitor::ExportXMLMetrics(std::ostream& os)
{
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    os << "<metrics>\n";
    os << "<functions>\n";
    for (const auto& f : functions) {
        os << "<function name=\"" + EscapeXML(f.first) << "\">\n";
        for (const auto& m : f.second) {
            m->ExportXML(os);
        }
        for (const auto& m : ctx_vis_fn) {
            m->ExportXML(f.first, os);
        }
        os << "</function>\n";
    }
    os << "</functions>\n";

    os << "<oop>\n";
    for (const auto& cl : classes) {
        os << "<class name=\"" + EscapeXML(cl) << "\">\n";
        for (const auto& cvis : ctx_vis_cl) {
            cvis->ExportXML(cl, os);
        }
        os << "</class>\n";
    }
    os << "</oop>\n";
    os << "</metrics>\n";

    return os;
}

bool MetricVisitor::TraverseDecl(clang::Decl* decl)
{
    /* Skip files that are in system files */
    if (!decl || isInSystemHeader(context->getSourceManager(), decl->getLocation())) {
        return true;
    }
    /* We cannot skip function body even if this visitor only visits function headers, because functions
     * can contain classes */
    return RecursiveASTVisitor::TraverseDecl(decl);
}
