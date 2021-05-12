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
class StmtNPathVisitor : public clang::StmtVisitor<StmtNPathVisitor>
{
public:
    explicit StmtNPathVisitor(clang::ASTContext *ctx) : ctx(ctx){}
    void VisitStmt(clang::Stmt *stmt);
    void VisitCompoundStmt(clang::CompoundStmt *stmt);
    void Visit(clang::Stmt *stmt);
    void VisitIfStmt(clang::IfStmt *stmt);
    void VisitSwitchStmt(clang::SwitchStmt *stmt);
    void VisitWhileStmt(clang::WhileStmt *stmt);
    void VisitDoStmt(clang::DoStmt *stmt);
    void VisitExpr(clang::Expr *expr);
    void VisitCXXTryStmt(clang::CXXTryStmt *stmt);
//    void VisitCXXCatchStmt(clang::CXXCatchStmt *stmt);
    void VisitCXXForRangeStmt(clang::CXXForRangeStmt *stmt);
    void VisitForStmt(clang::ForStmt *stmt);
    void VisitConditionalOperator(clang::ConditionalOperator *op);
    void VisitLambdaExpr(clang::LambdaExpr *expr);
    void VisitCaseStmt(clang::CaseStmt *stmt);
    int CountLogicalOperators(clang::Stmt *stmt);
    /**
     * Returns calculated NPATH complexity.
     * @return - Calculated NPATH complexity.
     */
    int GetCount() const { return count + lambda_count; }
    void VisitReturnStmt(clang::ReturnStmt *stmt);
protected:
    clang::ASTContext *ctx;
    int count = 0;
    int lambda_count = 0;
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
    int GetValue() const { return count; }
private:
    int count{0};
};


#endif //METRICS_NPATHVISITOR_HPP
