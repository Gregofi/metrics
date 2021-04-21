#include "clang/AST/Decl.h"
#include "include/metrics/FansVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/Logging.hpp"
#include "include/CtxVisitor.hpp"


bool FansVisitor::VisitFunctionDecl(clang::FunctionDecl *d)
{
    if(ctx->getSourceManager().isInSystemHeader(d->getLocation())
        || !d->isThisDeclarationADefinition()) return true;
    ASTMatcherVisitor vis(ctx);
    vis.AddMatchers({callExpr().bind("call")}, &counter);

    counter.SetCurrFuncId(GetFunctionHead(d));
    vis.TraverseDecl(d);
    counter.LeaveFunction();
    return true;
}

bool FansVisitor::TraverseLambdaExpr(clang::LambdaExpr *e)
{
    return true;
}

void FansVisitor::CalcMetrics(clang::ASTContext *ctx)
{
    this->ctx = ctx;
    TraverseTranslationUnitDecl(ctx->getTranslationUnitDecl());
    this->ctx = nullptr;
}

std::ostream &FansVisitor::Export(const std::string &s, std::ostream &os) const
{
    os << "Fan-in:  " << FanIn(s)  << "\n";
    os << "Fan-out: " << FanOut(s) << "\n";
    return os;
}

std::ostream &FansVisitor::ExportXML(const std::string &s, std::ostream &os) const
{
    os << Tag("fan_in", FanIn(s)) << Tag("fan_out", FanOut(s));
    return os;
}

void FanCount::run(const MatchFinder::MatchResult &Result)
{
    if(!is_in_func) return;
    if(const auto *s = Result.Nodes.getNodeAs<clang::CallExpr>("call"))
    {
        fan_in[curr_func] += 1;
        if(s->getDirectCallee())
            fan_out[GetFunctionHead(s->getDirectCallee())] += 1;
    }
}

void FanCount::InitFunction(const std::string &s)
{
    if(!fan_out.count(s))
        fan_out[s];
    if(!fan_in.count(s))
        fan_in[s];
}
