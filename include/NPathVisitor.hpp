//
// Created by filip on 3/26/21.
//

#ifndef METRICS_NPATHVISITOR_HPP
#define METRICS_NPATHVISITOR_HPP

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/StmtVisitor.h"

#include "include/AbstractVisitor.hpp"

class StmtNPathVisitor : public clang::ConstStmtVisitor<StmtNPathVisitor>
{
public:
    void VisitStmt(const clang::Stmt *stmt);
    void VisitCompoundStmt(const clang::CompoundStmt *stmt);
    void Visit(const clang::Stmt *stmt) { ConstStmtVisitor<StmtNPathVisitor>::Visit(stmt); }
    void VisitIfStmt(const clang::IfStmt *stmt);
    void VisitSwitchStmt(const clang::SwitchStmt *stmt);
    void VisitWhileStmt(const clang::WhileStmt *stmt);
    void VisitDoStmt(const clang::DoStmt *stmt);
    void VisitExpr(const clang::Expr *stmt);
    void VisitTry(const clang::CXXTryStmt *stmt);
    void VisitCatch(const clang::CXXCatchStmt *stmt);
    int GetCount() const { return count; }

protected:
    int count = 0;
};

class NPathVisitor : public clang::RecursiveASTVisitor<NPathVisitor>, public AbstractVisitor
{
public:
    NPathVisitor(clang::ASTContext *ctx);
    virtual ~NPathVisitor() override = default;
    virtual void CalcMetrics(clang::Decl *decl) override;
    bool VisitFunctionDecl(clang::FunctionDecl *decl);
//    int HandleStmt(clang::IfStmt *stmt);
//    int HandleStmt(clang::WhileStmt *stmt);
//    int HandleStmt(clang::DoStmt *stmt);
//    int HandleStmt(clang::ForStmt *stmt);
//    int HandleStmt(clang::SwitchStmt *stmt);
//    int HandleStmt(clang::ConditionalOperator *stmt);
//    int HandleStmt(clang::Expr *stmt);
//    int HandleStmt(clang::CompoundStmt);
};


#endif //METRICS_NPATHVISITOR_HPP
