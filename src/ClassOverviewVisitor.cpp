#include "include/metrics/ClassOverviewVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/Logging.hpp"
#include "include/Utility.hpp"

bool ClassOverviewVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* decl)
{
    /* Skip declarations that have no body and that aren't in main file */
    if (!decl || !decl->isThisDeclarationADefinition() || isInSystemHeader(ctx->getSourceManager(), decl->getLocation())
        /* Also skip declarations which represents lambdas and classes that we already added(this happens if they are
         * included from multiple files */
        || decl->isLambda()
        || (!decl->isClass() && !decl->isStruct())) {
        return true;
    }
    LOG(decl->getQualifiedNameAsString());
    /* Create new class in map, this is important because if there is an empty class (class A{};), it
     * wouldn't be added otherwise */
    if (!classes.count(decl->getQualifiedNameAsString())) {
        classes[decl->getQualifiedNameAsString()];
    }
    for (const auto& base : decl->bases()) {
        /* Base can also be template argument, only count if its concrete class */
        if (base.getType()->getAsCXXRecordDecl()) {
            classes[base.getType()->getAsCXXRecordDecl()->getQualifiedNameAsString()].children_count += 1;
            classes[decl->getQualifiedNameAsString()].inheritance_chain.emplace_back(base.getType()->getAsCXXRecordDecl()->getQualifiedNameAsString());
        }
    }

    CalculateLorKiddMetrics(decl);

    return true;
}

bool ClassOverviewVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl* decl)
{
    if (!decl || !decl->isThisDeclarationADefinition()
        || isInSystemHeader(ctx->getSourceManager(), decl->getLocation()) || decl->getParent()->isLambda()) {
        return true;
    }
    ASTMatcherVisitor vis(ctx);
    MethodCallback callback(decl->getParent()->getQualifiedNameAsString(), &classes);
    vis.AddMatchers({ cxxMemberCallExpr().bind("member_call"), memberExpr().bind("member_access") }, &callback);
    vis.TraverseDecl(decl);
    classes[decl->getParent()->getQualifiedNameAsString()].functions.emplace_back(callback.GetInstanceVars());
    auto vars = callback.GetInstanceVars();
    return true;
}

int ClassOverviewVisitor::GetInheritanceChainLen(const std::string& s) const
{
    assert(classes.contains(s));
    const auto& chain = classes.at(s).inheritance_chain;
    if (chain.empty()) {
        return 0;
    }
    int res = 0;
    for (const auto& c : chain) {
        res = std::max(GetInheritanceChainLen(c) + 1, res);
    }
    return res;
}

bool ClassOverviewVisitor::Similar(const std::set<std::string>& s1, const std::set<std::string>& s2)
{
    for (auto it1 = s1.begin(), it2 = s2.begin(); it1 != s1.end() && it2 != s2.end();) {
        if (*it1 == *it2) {
            return true;
        }
        if (*it1 < *it2) {
            ++it1;
        } else {
            ++it2;
        }
    }
    return false;
}

int ClassOverviewVisitor::LackOfCohesion(const std::string& s) const
{
    assert(classes.contains(s));
    auto c = classes.at(s);

    c.functions.size();
    int LOC = 0;
    for (size_t i = 0; i < c.functions.size(); ++i) {
        for (size_t j = i + 1; j < c.functions.size(); ++j) {
            LOC += Similar(c.functions[i], c.functions[j]) ? -1 : 1;
        }
    }
    return std::max(LOC, 0);
}

void MethodCallback::run(const MatchFinder::MatchResult& Result)
{
    /* FIXME: The check for methodDecl is needed because it returns nullptr for member pointer calls, which
     * should be fixed in upcoming versions of Clang */
    if (const auto* call = Result.Nodes.getNodeAs<CXXMemberCallExpr>("member_call"); call != nullptr
        && call->getMethodDecl() != nullptr) {
        /* If this class calls method from other class its coupled with it. Check if the called
         * method is from other class then this one */
        if (const auto& s = call->getMethodDecl()->getParent()->getQualifiedNameAsString(); s != currClass) {
            (*classes)[currClass].fan_in.insert(s);
            (*classes)[s].fan_out.insert(currClass);
        }
    }
    if (const auto* access = Result.Nodes.getNodeAs<MemberExpr>("member_access")) {
        /* Check if member is from CXXClass (it can also be from enum or union) */
        if (access->getMemberDecl()->isCXXClassMember()) {
            const FieldDecl* d = llvm::dyn_cast<FieldDecl>(access->getMemberDecl());
            if (d) {
                std::string parent_name = d->getParent()->getQualifiedNameAsString();
                /* Get id of the class that the member belongs to, check its this class id */
                if (parent_name == currClass) {
                    instance_vars.insert(d->getNameAsString());
                }
            }
        }
    }
}

void ClassOverviewVisitor::CalculateLorKiddMetrics(clang::CXXRecordDecl* decl)
{
    auto name = decl->getQualifiedNameAsString();
    for (const auto& m : decl->methods()) {
        if (!m->isDefaulted()) {
            classes[name].methods_count += 1;
            classes[name].public_methods_count += m->getAccess() == clang::AccessSpecifier::AS_public;
            classes[name].overriden_methods_count += !m->overridden_methods().empty();
        }
    }
    for (const auto& d : decl->fields()) {
        if (d->isCXXInstanceMember()) {
            classes[name].public_instance_vars_count += d->getAccess() == clang::AccessSpecifier::AS_public;
            classes[name].instance_vars_count += 1;
        }
    }
}

void ClassOverviewVisitor::CalcMetrics(clang::ASTContext* ctx)
{
    this->ctx = ctx;
    TraverseDecl(ctx->getTranslationUnitDecl());
    this->ctx = nullptr;
}

std::ostream& ClassOverviewVisitor::Export(const std::string& s, std::ostream& os) const
{
    assert(classes.contains(s));
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
       << "  fan in: " << c.fan_in.size() << "\n"
       << "  fan out: " << c.fan_out.size() << "\n"
       << "  Lack of cohesion: " << LackOfCohesion(s) << "\n";
    return os;
}

template <typename T>
size_t UnionSize(const std::set<T>& s1, const std::set<T> s2)
{
    std::vector<T> res;
    std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(res));
    return res.size();
}

std::ostream& ClassOverviewVisitor::ExportXML(const std::string& s, std::ostream& os) const
{
    assert(classes.contains(s));
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
                  + Tag("fan_in", c.fan_in.size())
                  + Tag("fan_out", c.fan_out.size())
                  + Tag("coupling", UnionSize(c.fan_in, c.fan_out))
                  + Tag("lack_of_cohesion", LackOfCohesion(s)));
    return os;
}

bool ClassOverviewVisitor::TraverseDecl(clang::Decl* decl)
{
    /* Skip files that are in system files */
    if (!decl) {
        return true;
    }
    return RecursiveASTVisitor::TraverseDecl(decl);
}