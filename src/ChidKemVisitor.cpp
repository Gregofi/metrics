#include "include/metrics/ChidKemVisitor.hpp"
#include "include/Logging.hpp"
#include "include/ASTMatcherVisitor.hpp"

bool ChidKemVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *decl)
{
    if(!decl->isThisDeclarationADefinition())
        return true;
    for(const auto &base : decl->bases())
    {
        classes[base.getType()->getAsCXXRecordDecl()->getID()].children_count += 1;
        classes[decl->getID()].inheritance_chain.emplace_back(base.getType()->getAsCXXRecordDecl()->getID());
    }
    return true;
}

bool ChidKemVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl *decl)
{
    ASTMatcherVisitor vis(ctx);
    MethodCallback callback(decl->getID());
    vis.AddMatchers({cxxMemberCallExpr().bind("member_call"), memberExpr().bind("member_access")}, &callback);
    vis.TraverseDecl(decl);
    auto set = callback.GetClasses();
    classes[decl->getID()].couples.insert(set.begin(), set.end());
    return true;
}

void MethodCallback::run(const MatchFinder::MatchResult &Result)
{
    Decl *parent;
    if(const auto *call = Result.Nodes.getNodeAs<CXXMemberCallExpr>("member_call"))
    {
        if(long unsigned id = call->getMethodDecl()->getParent()->getID(); id != currFunctionID)
        {
            classes.emplace(id);
        }
    }
    if(const auto *access = Result.Nodes.getNodeAs<MemberExpr>("member_access"))
    {
        /** Check if member is from CXXClass (it can also be from enum or union) */
        if(access->getMemberDecl()->isCXXClassMember())
        {
            /** Get id of the class that the member belongs to, check its this class id */
            if(unsigned int id = access->getMemberDecl()->getType()->getAsCXXRecordDecl()->getID();
                currFunctionID == id)
                instance_vars.insert(id);
        }
    }
}
