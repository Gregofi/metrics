#include "clang/AST/Decl.h"
#include "include/metrics/FansVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/Logging.hpp"

FansVisitor::FansVisitor(clang::ASTContext *ctx) : FunctionCtxVisitor(ctx), vis(ctx)
{
    vis.AddMatchers({callExpr().bind("call")}, &counter);
}

bool FansVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
    if(!ctx->getSourceManager().isInMainFile(decl->getLocation())) return true;
    counter.SetCurrFuncId(decl->getID());
    vis.TraverseDecl(decl);
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
    if(const auto *s = Result.Nodes.getNodeAs<clang::CallExpr>("call"))
    {
        LOG(curr_func);
        LOG(s->getStmtClassName() << " :" << s->getCalleeDecl()->getID());
        fan_in[curr_func] += 1;
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
