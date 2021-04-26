#include "include/metrics/ClassOverviewVisitor.hpp"
#include "include/Logging.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/Utility.hpp"

bool ClassOverviewVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *decl)
{
    /* Skip declarations that have no body and that aren't in main file */
    if(!decl->isThisDeclarationADefinition() || ctx->getSourceManager().isInSystemHeader(decl->getLocation())
        /* Also skip declarations which represents lambdas and classes that we already added(this happens if they are
         * included from multiple files */
        || decl->isLambda() || classes.count(decl->getQualifiedNameAsString())
        || (!decl->isClass() && !decl->isStruct()))
        return true;
    /* Create new class in map, this is important because if there is an empty class (class A{};), it
     * wouldn't be added otherwise */
    if(!classes.count(decl->getQualifiedNameAsString()))
        classes[decl->getQualifiedNameAsString()];
    for(const auto &base : decl->bases())
    {
        /* Base can also be template argument, only count if its concrete class */
        if(base.getType()->getAsCXXRecordDecl())
        {
            classes[base.getType()->getAsCXXRecordDecl()->getQualifiedNameAsString()].children_count += 1;
            classes[decl->getQualifiedNameAsString()].inheritance_chain.emplace_back(base.getType()->getAsCXXRecordDecl()->getQualifiedNameAsString());
        }
    }

    CalculateLorKiddMetrics(decl);

    return true;
}

bool ClassOverviewVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl *decl)
{
    if(!decl->isThisDeclarationADefinition() || ctx->getSourceManager().isInSystemHeader(decl->getLocation()) || decl->getParent()->isLambda())
        return true;
    ASTMatcherVisitor vis(ctx);
    MethodCallback callback(decl->getParent()->getQualifiedNameAsString());
    vis.AddMatchers({cxxMemberCallExpr().bind("member_call"), memberExpr().bind("member_access")}, &callback);
    vis.TraverseDecl(decl);
    auto set = callback.GetClasses();
    classes[decl->getParent()->getQualifiedNameAsString()].couples.insert(set.begin(), set.end());
    auto vars = callback.GetInstanceVars();
    classes[decl->getParent()->getQualifiedNameAsString()].functions.push_back(vars);
    return true;
}

int ClassOverviewVisitor::GetInheritanceChainLen(const std::string &s) const
{
    const auto &chain = classes.at(s).inheritance_chain;
    if(chain.empty())
        return 0;
    int res = 0;
    for(const auto &c : chain)
        res = std::max(GetInheritanceChainLen(c) + 1, res);
    return res;
}

bool ClassOverviewVisitor::Similar(const std::set<unsigned long> &s1, const std::set<unsigned long> &s2)
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

int ClassOverviewVisitor::LackOfCohesion(const std::string &s) const
{
    auto c = classes.at(s);

    c.functions.size();
    int LOC = 0;
    for(size_t i = 0; i < c.functions.size(); ++ i)
        for(size_t j = i + 1; j < c.functions.size(); ++ j)
            LOC += Similar(c.functions[i], c.functions[j]) ? -1 : 1;
    return std::max(LOC, 0);
}

void MethodCallback::run(const MatchFinder::MatchResult &Result)
{
    if(const auto *call = Result.Nodes.getNodeAs<CXXMemberCallExpr>("member_call"))
    {
        /* If this class calls method from other class its coupled with it. Check if the called
         * method is indeed from other class then this one */
        if(const auto &s = call->getMethodDecl()->getParent()->getQualifiedNameAsString(); s != currClassID)
        {
            classes.insert(s);
        }
    }
    if(const auto *access = Result.Nodes.getNodeAs<MemberExpr>("member_access"))
    {
        /** Check if member is from CXXClass (it can also be from enum or union) */
        if(access->getMemberDecl()->isCXXClassMember())
        {
            /** Get id of the class that the member belongs to, check its this class id */
            if(const FieldDecl *d = llvm::dyn_cast<FieldDecl>(access->getMemberDecl());
                    d && d->getParent()->getQualifiedNameAsString() == currClassID)
            {
                instance_vars.insert(d->getID());
            }
        }
    }
}

void ClassOverviewVisitor::CalculateLorKiddMetrics(clang::CXXRecordDecl *decl)
{
    auto name = decl->getQualifiedNameAsString();
    for(const auto &m : decl->methods())
    {
        if(!m->isDefaulted())
        {
            classes[name].methods_count += 1;
            classes[name].public_methods_count += m->getAccess() == clang::AccessSpecifier::AS_public;
            classes[name].overriden_methods_count += !m->overridden_methods().empty();
        }
    }
    for(const auto &d: decl->fields())
    {
        if(d->isCXXInstanceMember())
        {
            classes[name].public_instance_vars_count +=  d->getAccess() == clang::AccessSpecifier::AS_public;
            classes[name].instance_vars_count += 1;
        }
    }
}

void ClassOverviewVisitor::CalcMetrics(clang::ASTContext *ctx)
{
    this->ctx = ctx;
    TraverseDecl(ctx->getTranslationUnitDecl());
    this->ctx = nullptr;
}

std::ostream &ClassOverviewVisitor::Export(const std::string &s, std::ostream &os) const
{
    const auto& c = classes.at(s);
    os << "Size:\n"
       << "  Number of methods: " << c.methods_count << ", " << c.public_methods_count << " are public.\n"
       << "  Number of attributes: " << c.instance_vars_count << ", " << c.public_instance_vars_count << " are public.\n"
       << "\n"
       << "Inheritance:\n"
       << "  Number of children: " << c.children_count << "\n"
       << "  Longest inheritance chain length: " << GetInheritanceChainLen(s) << "\n"
       << "  Overriden methods: " << c.overriden_methods_count << "\n"
       << "Other:\n"
       << "  Number of couples: " << c.couples.size() << "\n"
       << "  Lack of cohesion : " << LackOfCohesion(s) << "\n";
    return os;
}

std::ostream &ClassOverviewVisitor::ExportXML(const std::string &s, std::ostream &os) const
{
    const auto& c = classes.at(s);
    os << Tag("size",
            "\n"
            + Tag("methods", c.methods_count)
            + Tag("public_methods", c.public_methods_count)
            + Tag("attributes", c.instance_vars_count)
            + Tag("public_attributes", c.public_instance_vars_count))
       << Tag("inheritance",
            "\n"
            + Tag("children", c.children_count)
            + Tag("inheritance_chain", GetInheritanceChainLen(s))
            + Tag("overriden_methods", c.overriden_methods_count))
       << Tag("other",
             "\n"
             + Tag("couples", c.couples.size())
             + Tag("LOC", LackOfCohesion(s)));
    return os;
}

bool ClassOverviewVisitor::TraverseDecl(clang::Decl *decl)
{
    /* Skip files that are in system files */
    if(!decl || ctx->getSourceManager().isInSystemHeader(decl->getLocation()))
        return true;
    return RecursiveASTVisitor::TraverseDecl(decl);
}