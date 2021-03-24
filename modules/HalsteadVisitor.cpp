//
// Created by filip on 3/22/21.
//

#include <array>

#include "HalsteadVisitor.hpp"


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

        declRefExpr().bind("stmt"),
};

const std::vector<clang::ast_matchers::DeclarationMatcher> HalsteadVisitor::operators_decl {

};

HalsteadVisitor::HalsteadVisitor(clang::ASTContext *ctx) : AbstractVisitor(ctx), matcher(ctx)
{
    const static std::vector<StatementMatcher> operands_stmt {
        declRefExpr(), /* This is an expression that refers to declaration, ie in 'if(x)' it'll match 'x' */
        /* literals */
        integerLiteral(),
        stringLiteral(),
        floatLiteral(),
        cxxNullPtrLiteralExpr(),
        cxxBoolLiteral(),
        characterLiteral(),
    };
    matcher.AddMatchers(operators_stmt, &tk_operators);
}
