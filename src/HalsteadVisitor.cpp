#include <array>

#include "include/metrics/HalsteadVisitor.hpp"


using namespace clang;


const std::vector<StatementMatcher> HalsteadVisitor::operators_stmt {
        /* Control flow statements */
        ifStmt().bind("stmt"),
        forStmt().bind("stmt"),
        gotoStmt().bind("stmt"),
        doStmt().bind("stmt"),
        switchStmt().bind("stmt"),
        whileStmt().bind("stmt"),
        cxxTryStmt().bind("stmt"),
        cxxCatchStmt().bind("stmt"),
        switchStmt().bind("stmt"),
        caseStmt().bind("stmt"),
        breakStmt().bind("stmt"),
        continueStmt().bind("stmt"),
        cxxForRangeStmt().bind("stmt"),
        cxxCatchStmt().bind("stmt"),
        cxxThrowExpr().bind("stmt"),
        defaultStmt().bind("stmt"),
        returnStmt().bind("stmt"),
        /* operators */
        binaryOperator().bind("stmt"),
        conditionalOperator().bind("stmt"),
        binaryConditionalOperator().bind("stmt"),
        unaryOperator().bind("stmt"),
        cxxOperatorCallExpr(), /* Overloaded operators */
        cxxNewExpr().bind("stmt"),
        cxxDeleteExpr().bind("stmt"),
        /* function calls */
        callExpr().bind("stmt"),
        /* explicit casts */
        explicitCastExpr().bind("stmt"),
};

const std::vector<clang::ast_matchers::DeclarationMatcher> HalsteadVisitor::operators_decl {
    varDecl().bind("decl"),
};

const std::vector<clang::ast_matchers::StatementMatcher> HalsteadVisitor::operands_stmt {
    /* All literals */
    integerLiteral().bind("stmt"),
    stringLiteral().bind("stmt"),
    characterLiteral().bind("stmt"),
    floatLiteral().bind("stmt"),
    cxxBoolLiteral().bind("stmt"),
    cxxNullPtrLiteralExpr().bind("stmt"),

    declRefExpr().bind("stmt"),
    labelStmt().bind("stmt"),
    /* 'goto' is operator, but this statement also covers the label */
    gotoStmt().bind("stmt"),

};

const std::vector<clang::ast_matchers::DeclarationMatcher> HalsteadVisitor::operands_decl {
    varDecl().bind("decl"),
};

HalsteadVisitor::HalsteadVisitor(clang::ASTContext *ctx) : FunctionVisitor(ctx), matcher(ctx), tk_operators(true)
{
    matcher.AddMatchers(operators_stmt, &tk_operators);
    matcher.AddMatchers(operators_decl, &tk_operators);

    matcher.AddMatchers(operands_stmt, &tk_operand);
    matcher.AddMatchers(operands_decl, &tk_operand);
}

void TokenCounter::run(const MatchFinder::MatchResult &Result)
{
    count += 1;
    if(const auto *s = Result.Nodes.getNodeAs<clang::Stmt>("stmt"))
    {
        seen_tokens_stmt.emplace(s->getStmtClass());
    }
    if(const Type *t = Result.Nodes.getNodeAs<clang::Type>("type"))
    {
        seen_tokens_type.emplace(t->getTypeClass());
    }
    if(const Decl *d = Result.Nodes.getNodeAs<clang::Decl>("decl"))
    {
        seen_tokens_decl.emplace(d->getKind());
        if(const auto *vardecl = llvm::dyn_cast<clang::VarDecl>(d); isOperator && vardecl)
        {
            count += vardecl->hasInit();
        }
    }
}
