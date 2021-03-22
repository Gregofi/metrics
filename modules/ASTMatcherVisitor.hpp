//
// Created by filip on 2/28/21.
//

#ifndef METRICS_ASTMATCHERVISITOR_HPP
#define METRICS_ASTMATCHERVISITOR_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

/**
 * AST Visitor that visits every node in given subtree and tries to match
 * it with given matchers.
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
     * @param callback
     * @param matchers
     */
    ASTMatcherVisitor(ASTContext *context, MatchFinder::MatchCallback *callback, std::vector<StatementMatcher> matchers) :
                            context(context), callback(callback)
    {
        for(const auto & x : matchers)
            finder.addMatcher(x, callback);
    }


    bool VisitDecl(Decl *decl)
    {
        finder.match(*decl, *context);
        return true;
    }

    bool VisitStmt(Stmt *stmt)
    {
        finder.match(*stmt, *context);
        return true;
    }

    bool VisitType(Type *type)
    {
        finder.match(*type, *context);
        return true;
    }

protected:
    MatchFinder finder;
    MatchFinder::MatchCallback *callback;
    clang::ASTContext *context;
};

#endif //METRICS_ASTMATCHERVISITOR_HPP
