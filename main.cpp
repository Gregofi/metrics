#include <memory>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include "include/Logging.hpp"
#include "include/MetricVisitor.hpp"

using namespace clang;
using clang::Stmt;

/* Command line args */
static llvm::cl::OptionCategory MetricsCategory("metrics options");
static llvm::cl::opt<std::string> XMLOutputOpt("xml",
                                               llvm::cl::desc("App will export measured data to XML, specify name of output file"),
                                               llvm::cl::value_desc("filename"),
                                               llvm::cl::cat(MetricsCategory));

MetricVisitor g_metricVisitor;


/* ======================================================================================= */

class FunctionInfoConsumer : public clang::ASTConsumer
{
public:
    explicit FunctionInfoConsumer(ASTContext *context)  {}

    virtual void HandleTranslationUnit(clang::ASTContext &context) override
    {
        g_metricVisitor.CalcMetrics(&context);
    }
private:
};

class FunctionInfoAction : public clang::ASTFrontendAction
{
public:
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) override
    {
        /* Mute all clang output (ie. warning and error in compiling) */
//        Compiler.getDiagnostics().setClient(new IgnoringDiagConsumer());
        return std::make_unique<FunctionInfoConsumer>(&Compiler.getASTContext());
    }
};


int main(int argc, const char **argv)
{
    auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, MetricsCategory, llvm::cl::Required);
    if(!ExpectedParser)
    {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    clang::tooling::CommonOptionsParser &OptionsParser = ExpectedParser.get();
    clang::tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    /* Add link to clang libraries, path to this can be found by running 'clang --print-file-name=include' */
//    clang::tooling::ArgumentsAdjuster ardj1 =
//            clang::tooling::getInsertArgumentAdjuster("-I/usr/lib/clang/11.1.0/include");
//    Tool.appendArgumentsAdjuster(ardj1);
    Tool.run(clang::tooling::newFrontendActionFactory<FunctionInfoAction>().get());
    if(!XMLOutputOpt.empty())
    {
        std::ofstream of(XMLOutputOpt.c_str());
        if(!of.good())
        {
            std::cerr << "Unable to write output to file, aborting" << std::endl;
            return 1;
        }
        g_metricVisitor.ExportXMLMetrics(of);
    }
    else
    {
        g_metricVisitor.ExportMetrics(std::cout);
    }
}

