
#include "clang/AST/Decl.h"
#include "include/metrics/FansVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"


FansVisitor::FansVisitor(clang::ASTContext *ctx) : ctx(ctx), vis(ctx)
{
    vis.AddMatchers({callExpr().bind("call"), cxxOperatorCallExpr().bind("call"), cxxMemberCallExpr().bind("call")}, &counter);
}

bool FansVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
    counter.SetCurrFuncId(decl->getID());
    vis.TraverseDecl(decl);
    return true;
}

bool FansVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl *decl)
{
    counter.SetCurrFuncId(decl->getID());
    vis.TraverseDecl(decl);
    return true;

}

void FanCount::run(const MatchFinder::MatchResult &Result)
{
    if(const auto *s = Result.Nodes.getNodeAs<clang::CallExpr>("call"))
    {
        fan_in[curr_func] += 1;
        fan_out[s->getCalleeDecl()->getID()] += 1;
    }
    if(const auto *s = Result.Nodes.getNodeAs<clang::CXXOperatorCallExpr>("call"))
    {
        fan_in[curr_func] += 1;
        fan_out[s->getCalleeDecl()->getID()] += 1;
    }
    if(const auto *s = Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("call"))
    {
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
