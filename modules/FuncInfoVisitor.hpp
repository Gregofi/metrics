//
// Created by filip on 12/29/20.
//

#ifndef METRICS_FUNCINFO_HPP
#define METRICS_FUNCINFO_HPP

//
// Created by filip on 12/29/20.
//

#include <fstream>
#include <set>
#include <array>
#include <algorithm>
#include <cassert>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using clang::Stmt;

struct Function
{
    std::string name;
    int length;
    int stmtCnt;
    int maxDepth;
};

class FuncInfoVisitor : public RecursiveASTVisitor<FuncInfoVisitor>
{
    /* Statements that contains other statements */
    static const std::array<Stmt::StmtClass, 10> compoundStatements;
    struct Result
    {
        int statements;
        int depth;
    };

public:
    explicit FuncInfoVisitor(ASTContext *context)
            : context(context) {}

    /**
     * Calculates number of lines for given function body.
     * @param decl
     * @return
     *
     * This calculates real number of lines, empty lines and comments are also counted.
     */
    int CalcLength(FunctionDecl *decl);

    /**
     * Handles calculation of depth and statements for if statement.
     * @param stmt - If statement
     * @param depth - current depth
     * @return - Result struct containing maximum depth of this statement and number of statements in this statement.
     *
     * If statement needs to be done separately, because the 'else' branch is child of the 'if' statement,
     * and depth would not be calculated properly.
     */
    Result HandleIfStatement(const Stmt *stmt, int depth);

    Result HandleOtherCompounds(const Stmt *body, int depth);

    Result StmtCount(const Stmt *body, int depth = 0);

    /**
     * Calculates range of function declaration.
     * @param decl
     * @return
     */
    bool VisitFunctionDecl(FunctionDecl *decl);

    std::map<int, Function> GetFunctions();

private:
    ASTContext *context;
    std::map<int, Function> funcs;
};

#endif //METRICS_FUNCINFO_HPP
