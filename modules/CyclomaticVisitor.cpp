
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "CyclomaticVisitor.hpp"
#include "ASTMatcherVisitor.hpp"


using namespace clang::ast_matchers;

     
class Counter : public MatchFinder::MatchCallback
{
public :
    virtual void run(const MatchFinder::MatchResult &Result) override
    {
        count += 1;
    }
    int getCount() const { return count; }
private:
    int count = 0;
};

bool CyclomaticVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
    Counter counter;
    std::vector<StatementMatcher> matchers
    = {
            clang::ast_matchers::forStmt(),
            clang::ast_matchers::binaryConditionalOperator(),
            clang::ast_matchers::whileStmt(),
            clang::ast_matchers::cxxTryStmt(),
            clang::ast_matchers::caseStmt(),
            clang::ast_matchers::doStmt(),
            clang::ast_matchers::ifStmt(),
            clang::ast_matchers::binaryOperator(hasOperatorName("&&")),
            clang::ast_matchers::binaryOperator(hasOperatorName("||"))
    };
    ASTMatcherVisitor v(context, &counter, matchers);
    v.TraverseDecl(decl);
    res.push_back({"Cyclomatic complexity", counter.getCount()});
    return true;
}

bool CyclomaticVisitor::VisitIfStmt(clang::IfStmt *decl)
{
    return true;
}
