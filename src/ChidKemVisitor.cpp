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
    if(!decl->isThisDeclarationADefinition() )
        return true;
    ASTMatcherVisitor vis(ctx);
    MethodCallback callback(decl->getParent()->getID());
    vis.AddMatchers({cxxMemberCallExpr().bind("member_call"), memberExpr().bind("member_access")}, &callback);
    vis.TraverseDecl(decl);
    auto set = callback.GetClasses();
    classes[decl->getParent()->getID()].couples.insert(set.begin(), set.end());
    auto vars = callback.GetInstanceVars();
    classes[decl->getParent()->getID()].functions.push_back(vars);
    return true;
}

int ChidKemVisitor::GetInheritanceChainLen(unsigned long id) const
{
    const auto &chain = classes.at(id).inheritance_chain;
    if(chain.empty())
        return 0;
    int res = 0;
    for(const auto &c : chain)
        res = std::max(GetInheritanceChainLen(c) + 1, res);
    return res;
}

bool ChidKemVisitor::Similiar(const std::set<unsigned long> &s1, const std::set<unsigned long> &s2)
{
    for(auto it1 = s1.begin(), it2 = s2.begin(); it1 != s1.end() && it2 != s2.end();)
    {
        if(*it1 == *it2)
            return true;
        if(*it1 < *it2)
            ++it1;
        else
            ++it2;
    }
    return false;
}

int ChidKemVisitor::LackOfCohesion(unsigned long id) const
{
    auto c = classes.at(id);

    c.functions.size();
    int LOC = 0;
    for(size_t i = 0; i < c.functions.size(); ++ i)
        for(size_t j = i + 1; j < c.functions.size(); ++ j)
            LOC += Similiar(c.functions[i], c.functions[j]) ? -1 : 1;
    return std::max(LOC, 0);
}

void MethodCallback::run(const MatchFinder::MatchResult &Result)
{
    Decl *parent;
    if(const auto *call = Result.Nodes.getNodeAs<CXXMemberCallExpr>("member_call"))
    {
        /* If this class calls method from other class its coupled with it. Check if the called
         * method is indeed from other class then this one */
        if(long unsigned id = call->getMethodDecl()->getParent()->getID(); id != currClassID)
        {
            classes.insert(id);
        }
    }
    if(const auto *access = Result.Nodes.getNodeAs<MemberExpr>("member_access"))
    {
        /** Check if member is from CXXClass (it can also be from enum or union) */
        if(access->getMemberDecl()->isCXXClassMember())
        {
            LOG("matched CXXMember access " << access->getMemberDecl()->getNameAsString());
            /** Get id of the class that the member belongs to, check its this class id */
            if(const FieldDecl *d = llvm::dyn_cast<FieldDecl>(access->getMemberDecl()); d && d->getParent()->getID() == currClassID)
            {
                instance_vars.insert(d->getID());
            }
        }
    }
}
