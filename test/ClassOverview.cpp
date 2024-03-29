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
    ASSERT_EQ(vis.GetRefClass("foreigner").fan_in.size(), 0);
    ASSERT_EQ(vis.GetRefClass("foreigner").fan_out.size(), 1);
    ASSERT_EQ(vis.GetRefClass("bar").fan_in.size(), 1);
    ASSERT_EQ(vis.GetRefClass("bar").fan_out.size(), 0);

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
    ASSERT_EQ(vis.GetRefClass("a").functions[2].count("a"), 1);
    ASSERT_EQ(vis.GetRefClass("a").functions[2].count("b"), 1);
    ASSERT_EQ(vis.GetRefClass("a").functions[0].size(), 3);
    ASSERT_EQ(vis.GetRefClass("a").functions[1].size(), 1);
    ASSERT_EQ(vis.GetRefClass("a").functions[1].count("c"), 1);

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

const char *fans = R"(
class A{public: void a() {} };
class B{public: void b() {} };
class C{public: void c() {} };

class Foo
{
    public:
    void f1(A *a)
    {
        a->a();
        B b;
        b.b();
        bb.b();
    }

    void f2(B *b)
    {
        b->b();
        bb.b();
    }

    B bb;
};

class Bar
{
    Foo f;
    void f1(C *c, A *a)
    {
        f.f1(a);
        c->c();
        f.f1(a);
    }
};
)";

int FansTest()
{
    auto v = Eval(fans);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    ASSERT_EQ(vis.GetRefClass("Foo").fan_in.size(), 2);
    ASSERT_EQ(vis.GetRefClass("Foo").fan_out.size(), 1);
    ASSERT_EQ(vis.GetRefClass("A").fan_out.size(), 1);
    ASSERT_EQ(vis.GetRefClass("B").fan_out.size(), 1);
    ASSERT_EQ(vis.GetRefClass("Bar").fan_in.size(), 2);
    ASSERT_EQ(vis.GetRefClass("Bar").fan_out.size(), 0);

    return 0;
}

const char* pointer_to_member = R"(
    class X {
        public:
        int foo() { return 1; }
    };

    class Y {
        public:
        int bar() {
            auto method_ptr = &X::foo;
            X x;
            auto X_ptr = &x;
            return (X_ptr->*method_ptr)();
        }
    };
)";

int PtrToMemberTest()
{
    auto v = Eval(pointer_to_member);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    // FIXME: Should be one, but because of clang not properly
    // handling ->* it is not implemented for now.
    // The test exists to check that handling ->* doesn't cause segfault.
    // ASSERT_EQ(vis.GetRefClass("X").fan_out.size(), 1);

    return 0;
}

int main()
{
    ASSERT_EQ(ClassOverviewVisitor::Similar({"a", "b", "c"}, {"d", "c", "w"}), true);
    ASSERT_EQ(ClassOverviewVisitor::Similar({"a", "b", "c"}, {"0", "b", "6"}), true);
    ASSERT_EQ(ClassOverviewVisitor::Similar({"a", "b", "d"}, {"c", "f"}), false);
    ASSERT_EQ(ClassOverviewVisitor::Similar({}, {"a"}), false);

    TEST(BasicCasesTest);
    TEST(InheritanceChainTest);
    TEST(MemberAccessTest);
    TEST(LackOfCohesionTest);
    TEST(InnerClassTest);
    TEST(FansTest);
    TEST(PtrToMemberTest);
    return 0;
}
