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

#include "include/FunctionVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"

class StmtNPathVisitor : public clang::StmtVisitor<StmtNPathVisitor>
{
public:
    explicit StmtNPathVisitor(clang::ASTContext *ctx) : ctx(ctx){}
    void VisitStmt(clang::Stmt *stmt);
    void VisitCompoundStmt(clang::CompoundStmt *stmt);
    void Visit(clang::Stmt *stmt) { StmtVisitor<StmtNPathVisitor>::Visit(stmt); }
    void VisitIfStmt(clang::IfStmt *stmt);
    void VisitSwitchStmt(clang::SwitchStmt *stmt);
    void VisitWhileStmt(clang::WhileStmt *stmt);
    void VisitDoStmt(clang::DoStmt *stmt);
    void VisitExpr(clang::Expr *stmt);
    void VisitCXXTryStmt(clang::CXXTryStmt *stmt);
    void VisitCXXCatchStmt(clang::CXXCatchStmt *stmt);
    void VisitForStmt(clang::ForStmt *stmt);
    int CountLogicalOperators(clang::Stmt *stmt);
    int GetCount() const { return count; }
    void VisitReturnStmt(clang::ReturnStmt *stmt);

protected:
    clang::ASTContext *ctx;
    int count = 0;
};

class NPathVisitor : public clang::RecursiveASTVisitor<NPathVisitor>, public FunctionVisitor
{
public:
    NPathVisitor(clang::ASTContext *ctx);
    virtual ~NPathVisitor() override = default;
    virtual void CalcMetrics(clang::Decl *decl) override;
    bool VisitFunctionDecl(clang::FunctionDecl *decl);
};


#endif //METRICS_NPATHVISITOR_HPP
