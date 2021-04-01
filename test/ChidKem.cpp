#include <clang/Tooling/Tooling.h>
#include "include/metrics/ChidKemVisitor.hpp"
#include "test/TestToolRun.hpp"
#include "test/Asserts.hpp"


const char *code1 = R"(
class foreigner
{
public:
    void kil() const {}
    int member;
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

struct Info
{
    CGetNames names;
    ChidKemVisitor vis;
};

Info Eval(const std::string &code)
{
    auto AST = clang::tooling::buildASTFromCode(code);
    ChidKemVisitor vis(&AST->getASTContext());
    vis.TraverseDecl(AST->getASTContext().getTranslationUnitDecl());
    CGetNames names;
    names.TraverseDecl(AST->getASTContext().getTranslationUnitDecl());
    return {names, vis};
}

int BasicCasesTest()
{
    auto v = Eval(code1);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    ASSERT_EQ(vis.GetConstClass(cnames["foo"]).children_count, 1);
    ASSERT_EQ(vis.GetConstClass(cnames["bar"]).children_count, 0);
    ASSERT_EQ(vis.GetConstClass(cnames["foreigner"]).children_count, 0);

    ASSERT_EQ(vis.GetConstClass(cnames["bar"]).inheritance_chain.front(), cnames["foo"]);
    ASSERT_EQ(vis.GetConstClass(cnames["bar"]).functions[0].size(), 2);
    ASSERT_EQ(vis.GetInheritanceChainLen(cnames["bar"]), 1);
    return 0;
}

int InheritanceChainTest()
{
    auto v = Eval(inh_chain);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    ASSERT_EQ(vis.GetInheritanceChainLen(cnames["c"]), 2);
    ASSERT_EQ(vis.GetInheritanceChainLen(cnames["b"]), 1);
    ASSERT_EQ(vis.GetInheritanceChainLen(cnames["a"]), 0);
    ASSERT_EQ(vis.GetInheritanceChainLen(cnames["d"]), 3);

    ASSERT_EQ(vis.GetConstClass(cnames["a"]).children_count, 2);
    ASSERT_EQ(vis.GetConstClass(cnames["b"]).children_count, 1);
    ASSERT_EQ(vis.GetConstClass(cnames["c"]).children_count, 1);
    ASSERT_EQ(vis.GetConstClass(cnames["d"]).children_count, 0);

    return 0;
}

int MemberAccessTest()
{
    auto v = Eval(members);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    ASSERT_EQ(vis.GetConstClass(cnames["a"]).functions[2].size(), 2);
    ASSERT_EQ(vis.GetConstClass(cnames["a"]).functions[0].size(), 3);
    ASSERT_EQ(vis.GetConstClass(cnames["a"]).functions[1].size(), 1);

    ASSERT_EQ(vis.LackOfCohesion(cnames["a"]), 0);
    return 0;
};

int LackOfCohesionTest()
{
    auto v = Eval(cohesion);
    auto cnames = v.names.class_names;
    auto fnames = v.names.func_names;
    auto vis = v.vis;

    ASSERT_EQ(vis.LackOfCohesion(cnames["a"]), 4);
    return 0;
}

int main()
{
    ASSERT_EQ(ChidKemVisitor::Similiar({1,2,3}, {2,5,6}), true);
    ASSERT_EQ(ChidKemVisitor::Similiar({1,2,3}, {0,3,6}), true);
    ASSERT_EQ(ChidKemVisitor::Similiar({1,2,4}, {3,5}), false);
    ASSERT_EQ(ChidKemVisitor::Similiar({}, {1}), false);

    TEST(BasicCasesTest);
    TEST(InheritanceChainTest);
    TEST(MemberAccessTest);
    TEST(LackOfCohesionTest);
    return 0;
}
