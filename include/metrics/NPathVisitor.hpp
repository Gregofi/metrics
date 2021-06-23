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
/**
 * Visitor which visits statements and calculates NPath complexity for given compound statement.
 */
class StmtNPathVisitor : public clang::StmtVisitor<StmtNPathVisitor, long long unsigned>
{
public:
    explicit StmtNPathVisitor(clang::ASTContext *ctx) : ctx(ctx){}

    unsigned long long VisitStmt(clang::Stmt *stmt);
    unsigned long long VisitCompoundStmt(clang::CompoundStmt *stmt);
    unsigned long long Visit(clang::Stmt *stmt);
    unsigned long long VisitIfStmt(clang::IfStmt *stmt);
    unsigned long long VisitSwitchStmt(clang::SwitchStmt *stmt);
    unsigned long long VisitWhileStmt(clang::WhileStmt *stmt);
    unsigned long long VisitDoStmt(clang::DoStmt *stmt);
    unsigned long long VisitExpr(clang::Expr *expr);
    unsigned long long VisitCXXTryStmt(clang::CXXTryStmt *stmt);
    unsigned long long VisitCXXForRangeStmt(clang::CXXForRangeStmt *stmt);
    unsigned long long VisitForStmt(clang::ForStmt *stmt);
    unsigned long long VisitConditionalOperator(clang::ConditionalOperator *op);
    unsigned long long VisitLambdaExpr(clang::LambdaExpr *expr);
    unsigned long long VisitCaseStmt(clang::CaseStmt *stmt);
    unsigned long long VisitReturnStmt(clang::ReturnStmt *stmt);

    int CountLogicalOperators(clang::Stmt *stmt);
    /**
     * Returns calculated NPATH complexity.
     * @return - Calculated NPATH complexity.
     */
    long long unsigned GetCount(clang::Stmt *stmt) { return Visit(stmt) + lambda_count; }
protected:
    clang::ASTContext *ctx;
    long long unsigned count = 1;
    long long unsigned lambda_count = 0;
};

/**
 * Calculates NPATH complexity for given function declaration.
 */
class NPathVisitor : public FunctionVisitor
{
public:
    NPathVisitor(clang::ASTContext *ctx);
    virtual ~NPathVisitor() override = default;
    virtual void CalcMetrics(clang::FunctionDecl *decl) override;
    virtual std::ostream &Export(std::ostream &os) const override;
    virtual std::ostream &ExportXML(std::ostream &os) const override;
    bool VisitFunctionDecl(clang::FunctionDecl *decl);
    long long unsigned GetValue() const { return count; }
private:
    long long unsigned count{0};
};


#endif //METRICS_NPATHVISITOR_HPP
