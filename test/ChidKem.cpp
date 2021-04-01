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

int main()
{
    TEST(BasicCasesTest);
    TEST(InheritanceChainTest);
    return 0;
}
