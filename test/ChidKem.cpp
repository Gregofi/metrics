

#include <clang/Tooling/Tooling.h>
#include "include/metrics/ChidKemVisitor.hpp"
#include "test/TestToolRun.hpp"

int main()
{
    auto AST = clang::tooling::buildASTFromCode(std::string("class foreigner{public:    void kil() const    {    }};class foo{};class bar : public foo{public:    void doit() const    {        int b = a + 2;        f.kil();    }protected:    foreigner f;    int a = 5;};int main(){}"));
    ChidKemVisitor vis(&AST->getASTContext());
    vis.TraverseDecl(AST->getASTContext().getTranslationUnitDecl());
    CGetNames names;
    names.TraverseDecl(AST->getASTContext().getTranslationUnitDecl());;
    return 0;
}
