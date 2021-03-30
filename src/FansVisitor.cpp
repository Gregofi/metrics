
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
