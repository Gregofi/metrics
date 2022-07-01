#include "include/metrics/NPathVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/Logging.hpp"
#include <numeric>

using namespace clang;
using namespace clang::ast_matchers;

NPathVisitor::NPathVisitor(clang::ASTContext* ctx)
    : FunctionVisitor(ctx)
{
}

void NPathVisitor::CalcMetrics(clang::FunctionDecl* decl)
{
    StmtNPathVisitor visitor(context);
    count = visitor.GetCount(decl->getBody());
}

std::ostream& NPathVisitor::Export(std::ostream& os) const
{
    os << "NPATH: " << count << "\n";
    return os;
}

std::ostream& NPathVisitor::ExportXML(std::ostream& os) const
{
    os << Tag("npath", count);
    return os;
}

/*-----------------------------------------------------------------------------------*/

unsigned long long StmtNPathVisitor::VisitStmt(Stmt* stmt)
{
    return std::accumulate(stmt->child_begin(), stmt->child_end(), 1,
        [this](unsigned long long val, clang::Stmt* stmt) {
            if (!stmt) {
                return val;
            }
            return val *= Visit(stmt);
        });
}

unsigned long long StmtNPathVisitor::VisitCompoundStmt(clang::CompoundStmt* stmt)
{
    return VisitStmt(stmt);
}

unsigned long long StmtNPathVisitor::VisitExpr(clang::Expr* expr)
{
    long long unsigned result = 1;
    for (const auto& x : expr->children()) {
        if (!x) {
            continue;
        }
        result *= Visit(x);
        ;
    }
    /* Check if currently visited expression is logical and, or, ternary */
    if (auto* bin = llvm::dyn_cast<BinaryOperator>(expr); bin
        && (bin->getOpcode() == BinaryOperatorKind::BO_LAnd
            || bin->getOpcode() == BinaryOperatorKind::BO_LOr)) {
        result += 1;
    }
    return result;
}

unsigned long long StmtNPathVisitor::VisitDoStmt(clang::DoStmt* stmt)
{
    return CountLogicalOperators(stmt->getCond()) + Visit(stmt->getBody()) + 1;
}

unsigned long long StmtNPathVisitor::VisitConditionalOperator(clang::ConditionalOperator* op)
{
    return CountLogicalOperators(op->getCond()) + Visit(op->getTrueExpr()) + Visit(op->getFalseExpr());
}

unsigned long long StmtNPathVisitor::VisitWhileStmt(clang::WhileStmt* stmt)
{
    return CountLogicalOperators(stmt->getCond()) + Visit(stmt->getBody()) + 1;
}

unsigned long long StmtNPathVisitor::VisitCaseStmt(clang::CaseStmt* stmt)
{
    if (!stmt) {
        return 1;
    }
    return Visit(stmt->getSubStmt());
}

unsigned long long StmtNPathVisitor::VisitSwitchStmt(clang::SwitchStmt* stmt)
{
    long long unsigned result = 1;
    result += CountLogicalOperators(stmt->getCond());
    long long unsigned case_range = 0;
    for (auto* c : stmt->getBody()->children()) {
        if (c->getStmtClass() == Stmt::CaseStmtClass) {
            result += case_range;
            case_range = Visit(llvm::dyn_cast<CaseStmt>(c)->getSubStmt());
        } else {
            case_range *= Visit(c);
        }
    }
    result += case_range;
    return result;
}

unsigned long long StmtNPathVisitor::VisitIfStmt(clang::IfStmt* stmt)
{
    long long unsigned result = 1 - (stmt->getElse() != nullptr);
    result += CountLogicalOperators(stmt->getCond());
    result += Visit(stmt->getThen());

    if (decltype(stmt->getElse()) e = stmt->getElse()) {
        result += Visit(e);
    }

    return result;
}

unsigned long long StmtNPathVisitor::VisitForStmt(clang::ForStmt* stmt)
{
    return CountLogicalOperators(stmt->getInit()) + CountLogicalOperators(stmt->getCond())
        + CountLogicalOperators(stmt->getInc()) + Visit(stmt->getBody()) + 1;
}

unsigned long long StmtNPathVisitor::VisitCXXTryStmt(clang::CXXTryStmt* stmt)
{
    long long unsigned res = 0;
    if (!stmt) {
        return 1;
    }
    res += Visit(stmt->getTryBlock());
    ;
    for (size_t i = 0; i < stmt->getNumHandlers(); ++i) {
        res += Visit(llvm::dyn_cast_or_null<clang::CompoundStmt>(stmt->getHandler(i)->getHandlerBlock()));
        ;
    }
    return res;
}

int StmtNPathVisitor::CountLogicalOperators(clang::Stmt* stmt)
{
    if (!stmt)
        return 0;
    ASTMatcherVisitor vis(ctx);
    Counter c;
    vis.AddMatchers({ binaryOperator(hasOperatorName("&&")),
                        binaryOperator(hasOperatorName("and")),
                        binaryOperator(hasOperatorName("or")),
                        binaryOperator(hasOperatorName("||")) },
        &c);
    vis.TraverseStmt(stmt);
    return c.getCount();
}

unsigned long long StmtNPathVisitor::VisitReturnStmt(clang::ReturnStmt* stmt)
{
    return CountLogicalOperators(stmt->getRetValue()) + 1;
}

unsigned long long StmtNPathVisitor::VisitLambdaExpr(clang::LambdaExpr* expr)
{
    lambda_count += Visit(expr->getBody());
    return 1;
}

unsigned long long StmtNPathVisitor::VisitCXXForRangeStmt(clang::CXXForRangeStmt* stmt)
{
    return Visit(stmt->getBody()) + 1;
}

unsigned long long StmtNPathVisitor::Visit(clang::Stmt* stmt)
{
    if (!stmt) {
        return 1;
    }
    return StmtVisitor<StmtNPathVisitor, long long unsigned>::Visit(stmt);
}