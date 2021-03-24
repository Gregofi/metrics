
#ifndef METRICS_METRIC_HPP
#define METRICS_METRIC_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

/**
 * Simple callback class that counts how many times a match has been found.
 */
class Counter : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override
    {
        count += 1;
    }
    int getCount() const { return count; }
private:
    int count = 0;
};


struct Metric
{
    std::string name;
    int val;
};

#endif //METRICS_METRIC_HPP
