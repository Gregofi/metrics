#include "include/metrics/ChidKemVisitor.hpp"
#include "include/Logging.hpp"
#include "include/ASTMatcherVisitor.hpp"

bool ChidKemVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *decl)
{
    if(!decl->isThisDeclarationADefinition() )
        return true;
    LOG(decl->getNameAsString());
    if(!classes.count(decl->getID()))
        classes[decl->getID()];
    for(const auto &base : decl->bases())
    {
        LOG("Class " << decl->getNameAsString() << " : parent " << base.getType()->getAsCXXRecordDecl()->getNameAsString());
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
    classes[decl->getParent()->getID()].couples.insert(set.begin(), set.end());
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
            if(const FieldDecl *d = llvm::dyn_cast<FieldDecl>(access->getMemberDecl());d && d->getParent()->getID() != currFunctionID)
                instance_vars.insert(d->getParent()->getID());
        }
    }
}
