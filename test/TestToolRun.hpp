#ifndef METRICS_TESTTOOLRUN_HPP
#define METRICS_TESTTOOLRUN_HPP

#include <fstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include "include/FunctionVisitor.hpp"
#include "include/ClassCtxVisitor.hpp"


/**
 * Testing function which takes the code, creates AST and returns metrics for given code.
 * Is aimed at metrics that only analyze single function. For example this isn't aimed
 * at Henry and Kafura fan-{in,out}, because these needs to know about other functions aswell.
 *
 * @tparam T_Visitor - FunctionVisitor to test
 * @param function_body - Body of the function from which AST will be created.
 *                        This is only body without opening { and } braces,
 *                        so if you want your function to look like this
 *                        int main(void) {if(a) return 1; return 0; }
 *                        enter only
 *                        if(a) return 1; return 0;
 * @return - Vector of calculated metrics
 */
template <typename T_Visitor>
T_Visitor ConstructMetricsOneFunction(const std::string &function_body)
{
    auto AST = clang::tooling::buildASTFromCode(std::string("int main(void) { " + function_body + " } "));
    T_Visitor vis(&AST->getASTContext());
    vis.CalcMetrics(AST->getASTContext().getTranslationUnitDecl());
    return vis;
}

/**
 * Class that should only be used for testing. Traverses AST and maps function
 * and class string names to their ids. Does not handle overloads.
 */
class CGetNames : public clang::RecursiveASTVisitor<CGetNames>
{
public:
    bool VisitFunctionDecl(clang::FunctionDecl *decl)
    {
        func_names[decl->getNameAsString()] = decl->getID();
        return true;
    }

    bool VisitMethodDecl(clang::CXXMethodDecl *decl)
    {
        func_names[decl->getNameAsString()] = decl->getID();
        return true;
    }

    bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl)
    {
        class_names[decl->getNameAsString()] = decl->getID();
        return true;
    }

    std::map<std::string, size_t> func_names;
    std::map<std::string, size_t> class_names;
};

std::map<std::string, size_t> GetFuncNameMap(clang::Decl *decl)
{
    CGetNames x;
    x.TraverseDecl(decl);
    return x.func_names;
}

#endif //METRICS_TESTTOOLRUN_HPP
