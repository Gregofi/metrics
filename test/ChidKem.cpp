

#include <clang/Tooling/Tooling.h>
#include "include/metrics/ChidKemVisitor.hpp"

int main()
{
    auto AST = clang::tooling::buildASTFromCode(std::string("class foo {}; class bar : public foo {}; int main(){}"));
    ChidKemVisitor vis(&AST->getASTContext());
    vis.TraverseDecl(AST->getASTContext().getTranslationUnitDecl());

    return 0;
}