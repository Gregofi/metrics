#include <fstream>
#include <set>
#include <array>
#include <algorithm>
#include <cassert>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "modules/CyclomaticVisitor.hpp"
#include "modules/FuncInfoVisitor.hpp"

using namespace clang;
using clang::Stmt;

/* ======================================================================================= */

class FunctionInfoConsumer : public clang::ASTConsumer
{
public:
    explicit FunctionInfoConsumer(ASTContext *context) : visitorFunc(context, functions), visitorCycl(context, functions) {}

    virtual void HandleTranslationUnit(clang::ASTContext &context)
    {
        visitorFunc.TraverseDecl(context.getTranslationUnitDecl());
        visitorCycl.TraverseDecl(context.getTranslationUnitDecl());
        /* DEBUG : Print all metrics to stdout as is */
        for(const auto & x : functions)
        {
            auto func = x.second;
            for(const auto & metric : func)
            llvm::outs() << metric.name << " : " << metric.val << "\n";
        }
    }
private:
    std::map<int64_t, Function> functions;
    FuncInfoVisitor visitorFunc;
    CyclomaticVisitor visitorCycl;
};

class FunctionInfoAction : public clang::ASTFrontendAction
{
public:
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
    {
        return std::unique_ptr<FunctionInfoConsumer>(new FunctionInfoConsumer(&Compiler.getASTContext()));
    }
};


int main(int argc, char **argv) {
    std::ifstream fs(argv[1]);
    std::string s, l;
    while(std::getline(fs, l))
        s += l + '\n';
    if (argc > 1) {
        clang::tooling::runToolOnCode(std::make_unique<FunctionInfoAction>(), s, argv[1]);
    }
}
