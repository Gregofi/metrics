
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
    FansVisitor vis(&AST->getASTContext());
    vis.TraverseDecl(AST->getASTContext().getTranslationUnitDecl());
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
    ASSERT_EQ(vis.FanIn(names["foo"]), 0);
    ASSERT_EQ(vis.FanOut(names["foo"]), 1);

    ASSERT_EQ(vis.FanIn(names["bar"]), 1);
    ASSERT_EQ(vis.FanOut(names["bar"]), 1);

    ASSERT_EQ(vis.FanIn(names["main"]), 1);
    ASSERT_EQ(vis.FanOut(names["main"]), 0);
    return 0;
}

int main()
{
    TEST(BasicTest);
}