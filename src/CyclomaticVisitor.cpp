#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "include/ASTMatcherVisitor.hpp"
#include "include/Utility.hpp"
#include "include/metrics/CyclomaticVisitor.hpp"

using namespace clang::ast_matchers;

CyclomaticVisitor::~CyclomaticVisitor()
{
}

CyclomaticVisitor::CyclomaticVisitor(clang::ASTContext* context)
    : FunctionVisitor(context)
{
}

std::ostream& CyclomaticVisitor::Export(std::ostream& os) const
{
    os << "Cyclomatic complexity: " << count << "\n";
    return os;
}

std::ostream& CyclomaticVisitor::ExportXML(std::ostream& os) const
{
    os << Tag("cyclomatic", count);
    return os;
}

void CyclomaticVisitor::CalcMetrics(clang::FunctionDecl* decl)
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
              clang::ast_matchers::cxxCatchStmt(),
              clang::ast_matchers::caseStmt(),
              clang::ast_matchers::doStmt(),
              clang::ast_matchers::ifStmt(),
              clang::ast_matchers::binaryOperator(hasOperatorName("&&")),
              clang::ast_matchers::binaryOperator(hasOperatorName("||")),
              clang::ast_matchers::gotoStmt(),
          };
    ASTMatcherVisitor v(context);
    v.AddMatchers(matchers, &counter);
    v.TraverseDecl(decl);
    count = counter.getCount() + 1;
}
