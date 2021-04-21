#include <clang/Tooling/Tooling.h>
#include "include/metrics/ClassOverviewVisitor.hpp"
#include "test/TestToolRun.hpp"
#include "test/Asserts.hpp"

const char *code1 = R"(
class foreigner
{
public:
    void kil() const {}
    int member;
private:
    int hidden;
    void hidden_m() {}
};

class foo{};

template<typename T>
class bar : public foo
{
public:
    void doit() const
    {
        int b = a + 2;
        f.kil();
        f.member;
    }
protected:
    foreigner f;
    int a = 5;
};

int main(){}
)";

struct Info
{
    Info(CGetNames names, ClassOverviewVisitor vis) : names(std::move(names)), vis(std::move(vis)){}

    CGetNames names;
    ClassOverviewVisitor vis;
};

Info Eval(const std::string &code)
{
    auto AST = clang::tooling::buildASTFromCode(code);
    ClassOverviewVisitor vis;
    vis.CalcMetrics(&AST->getASTContext());
    CGetNames names;
    names.TraverseDecl(AST->getASTContext().getTranslationUnitDecl());
    return {std::move(names), std::move(vis)};
}

int BasicCasesTest()
{
    auto v = Eval(code1);
    auto vis = v.vis;

    ASSERT_EQ(vis.GetRefClass("foo").children_count, 1);
    ASSERT_EQ(vis.GetRefClass("bar").children_count, 0);
    ASSERT_EQ(vis.GetRefClass("foreigner").children_count, 0);

    ASSERT_EQ(vis.GetRefClass("bar").inheritance_chain.front(), "foo");
    ASSERT_EQ(vis.GetRefClass("bar").functions[0].size(), 2);
    ASSERT_EQ(vis.GetInheritanceChainLen("bar"), 1);

    /* Test counts */
    ASSERT_EQ(vis.GetRefClass("foreigner").methods_count, 2);
    ASSERT_EQ(vis.GetRefClass("foreigner").public_methods_count, 1);
    ASSERT_EQ(vis.GetRefClass("foreigner").instance_vars_count, 2);
    ASSERT_EQ(vis.GetRefClass("foreigner").public_instance_vars_count, 1);
    ASSERT_EQ(vis.GetRefClass("foreigner").overriden_methods_count, 0);

    ASSERT_EQ(vis.GetRefClass("bar").methods_count, 1);
    ASSERT_EQ(vis.GetRefClass("bar").instance_vars_count, 2);
    ASSERT_EQ(vis.GetRefClass("bar").public_instance_vars_count, 0);
    return 0;
}

const char *inh_chain = R"(
class a
{
};

class b : a
{
};

class c : virtual b, virtual a
{
};

class d : c
{
};

int main(){}
)";

int InheritanceChainTest()
{
    auto v = Eval(inh_chain);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    ASSERT_EQ(vis.GetInheritanceChainLen("c"), 2);
    ASSERT_EQ(vis.GetInheritanceChainLen("b"), 1);
    ASSERT_EQ(vis.GetInheritanceChainLen("a"), 0);
    ASSERT_EQ(vis.GetInheritanceChainLen("d"), 3);

    ASSERT_EQ(vis.GetRefClass("a").children_count, 2);
    ASSERT_EQ(vis.GetRefClass("b").children_count, 1);
    ASSERT_EQ(vis.GetRefClass("c").children_count, 1);
    ASSERT_EQ(vis.GetRefClass("d").children_count, 0);

    return 0;
}

const char *members = R"(
class a
{
public:
    int foo();

    int bar()
    {
        int tmp = a * b;
        tmp = a + b;
        tmp += c;
        return tmp;
    }

    int rab()
    {
        return c;
    }
private:
    int a{}, b{}, c{};
};

int a::foo()
{
    int tmp = a * b;
    tmp += a;
    tmp += a * b;
    return tmp;
}
)";

int MemberAccessTest()
{
    auto v = Eval(members);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    ASSERT_EQ(vis.GetRefClass("a").functions[2].size(), 2);
    ASSERT_EQ(vis.GetRefClass("a").functions[0].size(), 3);
    ASSERT_EQ(vis.GetRefClass("a").functions[1].size(), 1);

    ASSERT_EQ(vis.LackOfCohesion("a"), 0);

    /* Test counts */
    ASSERT_EQ(vis.GetRefClass("a").instance_vars_count, 3);
    ASSERT_EQ(vis.GetRefClass("a").public_instance_vars_count, 0);
    ASSERT_EQ(vis.GetRefClass("a").methods_count, 3);
    ASSERT_EQ(vis.GetRefClass("a").public_methods_count, 3);

    return 0;
};

const char *cohesion = R"(
class a
{
public:
    int a, b, c, d;
    void f1() { a = 1; b = 1; }
    void f2() { a = 1; }
    void f3() {d = 3; }
    void f4() {c = 5; }
};
)";

int LackOfCohesionTest()
{
    auto v = Eval(cohesion);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    ASSERT_EQ(vis.LackOfCohesion("a"), 4);
    return 0;
}

const char *inner_classes = R"(
class a
{
public:
    int a, b, c;
    void f1() {}
private:
    int d;
    void f2() {}
    class b {
    public:
        void f3() {}
    private:
    int e;
    };
};

)";

int InnerClassTest()
{
    auto v = Eval(inner_classes);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    ASSERT_EQ(vis.GetRefClass("a").public_methods_count, 1);
    ASSERT_EQ(vis.GetRefClass("a").methods_count, 2);
    ASSERT_EQ(vis.GetRefClass("a").instance_vars_count, 4);
    ASSERT_EQ(vis.GetRefClass("a").public_instance_vars_count, 3);
    ASSERT_EQ(vis.GetRefClass("a::b").methods_count, 1);
    ASSERT_EQ(vis.GetRefClass("a::b").public_methods_count, 1);
    ASSERT_EQ(vis.GetRefClass("a::b").instance_vars_count, 1);
    ASSERT_EQ(vis.GetRefClass("a::b").public_instance_vars_count, 0);

    return 0;
}

int main()
{
    ASSERT_EQ(ClassOverviewVisitor::Similar({1, 2, 3}, {2, 5, 6}), true);
    ASSERT_EQ(ClassOverviewVisitor::Similar({1, 2, 3}, {0, 3, 6}), true);
    ASSERT_EQ(ClassOverviewVisitor::Similar({1, 2, 4}, {3, 5}), false);
    ASSERT_EQ(ClassOverviewVisitor::Similar({}, {1}), false);

    TEST(BasicCasesTest);
    TEST(InheritanceChainTest);
    TEST(MemberAccessTest);
    TEST(LackOfCohesionTest);
    TEST(InnerClassTest);
    return 0;
}
