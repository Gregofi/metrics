#include <memory>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include "modules/MetricVisitor.hpp"

using namespace clang;
using clang::Stmt;

/* ======================================================================================= */

class FunctionInfoConsumer : public clang::ASTConsumer
{
public:
    explicit FunctionInfoConsumer(ASTContext *context) : metricVisitor(context)  {}

    virtual void HandleTranslationUnit(clang::ASTContext &context) override
    {
        metricVisitor.TraverseDecl(context.getTranslationUnitDecl());
    }
private:
    MetricVisitor metricVisitor;
};

class FunctionInfoAction : public clang::ASTFrontendAction
{
public:
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) override
    {
        return std::make_unique<FunctionInfoConsumer>(&Compiler.getASTContext());
    }
};

static llvm::cl::OptionCategory MyToolCategory("metrics options");

int main(int argc, const char **argv)
{
    clang::tooling::CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    clang::tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    /* Add link to clang libraries, path to this can be found by running 'clang --print-file-name=include' */
    clang::tooling::ArgumentsAdjuster ardj1 =
            clang::tooling::getInsertArgumentAdjuster("-I/usr/lib/clang/11.1.0/include");
    Tool.appendArgumentsAdjuster(ardj1);

    Tool.run(clang::tooling::newFrontendActionFactory<FunctionInfoAction>().get());
}
