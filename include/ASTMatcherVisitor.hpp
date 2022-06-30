#pragma once

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;
using namespace clang;

/**
 * AST Visitor that visits every node in given subtree and tries to match
 * it with given matchers. To run it, call one of the Traverse* function that
 * it inherits from RecursiveASTVisitor.
 *
 * ---
 * There is a much easier way to do this, however it requires to edit
 * clang source code, because clang MatchFinder either visits whole AST
 * or only one node.
 *
 * Now it visits every statement and declaration and tries to match it with given matchers.
 */
class ASTMatcherVisitor : public clang::RecursiveASTVisitor<ASTMatcherVisitor>
{
public:
    /**
     * Creates visitor that visits every node, tries to match it with given matchers and
     * if it is matched, callback will be called.
     * @param context
     */
    ASTMatcherVisitor(ASTContext *context) : context(context)
    {
    }

    void AddMatchers(const std::vector<StatementMatcher>& matchers, MatchFinder::MatchCallback *callback)
    {
        for(const auto & x : matchers)
            finder.addMatcher(x, callback);
    }

    void AddMatchers(const std::vector<TypeMatcher>& matchers, MatchFinder::MatchCallback *callback)
    {
        for(const auto & x : matchers)
            finder.addMatcher(x, callback);
    }

    void AddMatchers(const std::vector<DeclarationMatcher>& matchers, MatchFinder::MatchCallback *callback)
    {
        for(const auto & x : matchers)
            finder.addMatcher(x, callback);
    }
    

    bool VisitDecl(const Decl *d)
    {
        finder.match(*d, *context);
        return true;
    }

    bool VisitStmt(const Stmt *s)
    {
        finder.match(*s, *context);
        return true;
    }

protected:
    MatchFinder finder;
    clang::ASTContext *context;
};
