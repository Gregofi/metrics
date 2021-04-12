#include "clang/AST/Decl.h"
#include "include/metrics/FansVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/Logging.hpp"
#include "include/CtxVisitor.hpp"

FansVisitor::FansVisitor(clang::ASTContext *ctx) : CtxVisitor(ctx), vis(ctx)
{
    vis.AddMatchers({callExpr().bind("call")}, &counter);
}

bool FansVisitor::VisitFunctionDecl(clang::FunctionDecl *d)
{
    if(!ctx->getSourceManager().isInMainFile(d->getLocation()) || !d->isThisDeclarationADefinition()) return true;
    counter.SetCurrFuncId(d->getID());
    vis.TraverseDecl(d);
    counter.LeaveFunction();
    return true;
}

void FansVisitor::CalcMetrics(clang::TranslationUnitDecl *decl)
{
    TraverseTranslationUnitDecl(decl);
}

std::ostream &FansVisitor::Export(size_t id, std::ostream &os) const
{
    os << "Fan-in:  " << FanIn(id)  << "\n";
    os << "Fan-out: " << FanOut(id) << "\n";
    return os;
}

void FanCount::run(const MatchFinder::MatchResult &Result)
{
    if(!is_in_func) return;
    if(const auto *s = Result.Nodes.getNodeAs<clang::CallExpr>("call"))
    {
        fan_in[curr_func] += 1;
        if(s->getCalleeDecl())
            fan_out[s->getCalleeDecl()->getID()] += 1;
    }
}

void FanCount::InitFunction(size_t id)
{
    if(!fan_out.count(id))
        fan_out[id];
    if(!fan_in.count(id))
        fan_in[id];
}
