#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "include/metrics/CyclomaticVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/Metric.hpp"

using namespace clang::ast_matchers;



bool CyclomaticVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
    Counter counter;
    /* Statements that make the code branch */
    const static std::vector<StatementMatcher> matchers
    = {
            clang::ast_matchers::forStmt(),
            clang::ast_matchers::binaryConditionalOperator(),
            clang::ast_matchers::conditionalOperator(),
            clang::ast_matchers::cxxForRangeStmt(),
            clang::ast_matchers::whileStmt(),
            clang::ast_matchers::cxxTryStmt(),
            clang::ast_matchers::caseStmt(),
            clang::ast_matchers::doStmt(),
            clang::ast_matchers::ifStmt(),
            clang::ast_matchers::binaryOperator(hasOperatorName("&&")),
            clang::ast_matchers::binaryOperator(hasOperatorName("||")),
    };
    ASTMatcherVisitor v(context);
    v.AddMatchers(matchers, &counter);
    v.TraverseDecl(decl);
    metrics.push_back({"Cyclomatic complexity", counter.getCount() + 1});
    return true;
}

CyclomaticVisitor::~CyclomaticVisitor()
{

}

CyclomaticVisitor::CyclomaticVisitor(clang::ASTContext *context) : FunctionVisitor(context)
{

}

