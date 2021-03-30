//
// Created by filip on 3/28/21.
//

#ifndef METRICS_TESTTOOLRUN_HPP
#define METRICS_TESTTOOLRUN_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include "include/FunctionVisitor.hpp"



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
std::vector<Metric> ConstructMetricsOneFunction(const std::string &function_body)
{
    auto AST = clang::tooling::buildASTFromCode(std::string("int main(void) { " + function_body + " } "));
    T_Visitor vis(&AST->getASTContext());
    vis.CalcMetrics(AST->getASTContext().getTranslationUnitDecl());
    return vis.GetMetrics();
}

#endif //METRICS_TESTTOOLRUN_HPP
