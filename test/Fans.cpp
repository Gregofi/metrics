#include "test/Asserts.hpp"
#include "test/TestToolRun.hpp"

#include "include/metrics/FansVisitor.hpp"


struct Info
{
    std::map<std::string, size_t> names;
    FansVisitor vis;
};

Info GetMetric(const std::string &code)
{
    auto AST = clang::tooling::buildASTFromCode(std::string(code));
    FansVisitor vis;
    vis.CalcMetrics(&AST->getASTContext());
    std::map<std::string, size_t> names = GetFuncNameMap(AST->getASTContext().getTranslationUnitDecl());
    return {names, vis};
}

int BasicTest()
{
    auto info = GetMetric(""
                          "void foo() {}"
                          "void bar() { foo();  }"
                          "int main() {bar(); }");
    auto names = info.names;
    auto vis   = info.vis;
    ASSERT_EQ(vis.FanIn("foo()"), 0);
    ASSERT_EQ(vis.FanOut("foo()"), 1);

    ASSERT_EQ(vis.FanIn("bar()"), 1);
    ASSERT_EQ(vis.FanOut("bar()"), 1);

    ASSERT_EQ(vis.FanIn("main()"), 1);
    ASSERT_EQ(vis.FanOut("main()"), 0);
    return 0;
}

int ClassesTest()
{
    auto info = GetMetric(R"(
    class Foo
    {
    public:
        Foo(int i) : i(i) {};
        void f2() {};
        void f1() & {i = 2;};
        void f1() const & {};
        void f1() && {i = 5;};
        void f1() const && {};
    private:
        int i;
    };

    void of1() {};
    void of2() {};
    int main() {
        Foo foo(1);
        foo.f1();
        foo.f2();
        of1();
        of2();

        Foo(1).f1();
        Foo(1).f1();

        static_cast<const Foo&&>(Foo(1)).f1();
        static_cast<const Foo&&>(Foo(1)).f1();
        static_cast<const Foo&&>(Foo(1)).f1();

        static_cast<const Foo&>(Foo(1)).f1();
        static_cast<const Foo&>(Foo(1)).f1();
        static_cast<const Foo&>(Foo(1)).f1();
        static_cast<const Foo&>(Foo(1)).f1();
    }
    )");
    auto names = info.names;
    auto vis = info.vis;
    ASSERT_EQ(vis.FanIn("main()"), 13);
    ASSERT_EQ(vis.FanOut("main()"),0);
    ASSERT_EQ(vis.FanOut("Foo::f1() &"), 1);
    ASSERT_EQ(vis.FanOut("Foo::f2()"), 1);
    ASSERT_EQ(vis.FanOut("of1()"), 1);
    ASSERT_EQ(vis.FanOut("of2()"), 1);
    ASSERT_EQ(vis.FanIn("Foo::f1() &"), 0);
    ASSERT_EQ(vis.FanIn("Foo::f2()"), 0);
    ASSERT_EQ(vis.FanIn("of1()"), 0);
    ASSERT_EQ(vis.FanIn("of2()"), 0);

    ASSERT_EQ(vis.FanOut("Foo::f1() &&"), 2);
    ASSERT_EQ(vis.FanOut("Foo::f1() const &&"), 3);
    ASSERT_EQ(vis.FanOut("Foo::f1() const &"), 4);
    return 0;
}

int main()
{
    TEST(BasicTest);
    TEST(ClassesTest);
}