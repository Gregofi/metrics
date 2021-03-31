#ifndef METRICS_PROJECT_CHIDAMBERKEMERERVISITOR_HPP
#define METRICS_PROJECT_CHIDAMBERKEMERERVISITOR_HPP

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <include/ASTMatcherVisitor.hpp>
#include "clang/AST/RecursiveASTVisitor.h"

typedef long unsigned FunctionID_t ;
typedef long unsigned ClassID_t ;

class MethodCallback : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    explicit MethodCallback(long unsigned currFunctionID) : currFunctionID(currFunctionID) {}
    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
    std::set<long unsigned> GetClasses() { return std::move(classes); }
protected:
    long unsigned currFunctionID;
    std::set<long unsigned> classes;
    std::set<long unsigned> instance_vars;
};

class ChidKemVisitor : public clang::RecursiveASTVisitor<ChidKemVisitor>
{
protected:
    struct Class
    {
        /**
         * Number of children for given Key
         */
        int children_count;
        /**
         * For given Key returns all functions, from which Key directly derives
         */
        std::vector<ClassID_t> inheritance_chain;

        /**
         * Contains classes whose methods this class calls
         */
        std::set<ClassID_t> couples;
    };

public:
    ChidKemVisitor(clang::ASTContext *ctx) : ctx(ctx) {}
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl);
    bool VisitCXXMethodDecl(clang::CXXMethodDecl *decl);
protected:
    std::map<ClassID_t, Class> classes;
    clang::ASTContext *ctx;
};

#endif //METRICS_PROJECT_CHIDAMBERKEMERERVISITOR_HPP
