#include "include/metrics/NPathVisitor.hpp"
#include "include/Logging.hpp"
#include "include/ASTMatcherVisitor.hpp"

using namespace clang;
using namespace clang::ast_matchers;

NPathVisitor::NPathVisitor(clang::ASTContext *ctx) : FunctionVisitor(ctx)
{

}

void NPathVisitor::CalcMetrics(clang::FunctionDecl *decl)
{
    StmtNPathVisitor visitor(context);
    visitor.Visit(decl->getBody());
    count = visitor.GetCount();
}

std::ostream &NPathVisitor::Export(std::ostream &os) const
{
    os << "NPATH: " << count << "\n";
    return os;
}

std::ostream &NPathVisitor::ExportXML(std::ostream &os) const
{
    os << Tag("npath", count);
    return os;
}

/*-----------------------------------------------------------------------------------*/

void StmtNPathVisitor::VisitStmt(Stmt *stmt)
{
    long long unsigned result = 1;
    for(const auto & x : stmt->children())
    {
        if(!x)
            continue;
        Visit(x);
        result *= count;
    }
    count = result;
}

void StmtNPathVisitor::VisitCompoundStmt(clang::CompoundStmt *stmt)
{
    long long unsigned result = 1;
    for(const auto &x : stmt->children())
    {
        if(!x)
            continue;
        Visit(x);
        result *= count;
    }
    count = result;
}

void StmtNPathVisitor::VisitExpr(clang::Expr *expr)
{
    long long unsigned result = 1;
    for(const auto & x : expr->children())
    {
        if(!x) continue;
        Visit(x);
        result *= count;
    }
    /* Check if currently visited expression is logical and, or, ternary */
    if(auto *bin = llvm::dyn_cast<BinaryOperator>(expr); bin
         && (bin->getOpcode() == BinaryOperatorKind::BO_LAnd
             || bin->getOpcode() == BinaryOperatorKind::BO_LOr))
        result += 1;
    count = result;
}

void StmtNPathVisitor::VisitDoStmt(clang::DoStmt *stmt)
{
    long long unsigned result = 1;
    result += CountLogicalOperators(stmt->getCond());
    Visit(stmt->getBody());
    result += count;
    count = result;
}

void StmtNPathVisitor::VisitConditionalOperator(clang::ConditionalOperator *op)
{
    long long unsigned result = CountLogicalOperators(op->getCond());
    Visit(op->getTrueExpr());
    result += count;
    Visit(op->getFalseExpr());
    result += count;
    count = result;
}

void StmtNPathVisitor::VisitWhileStmt(clang::WhileStmt *stmt)
{
    long long unsigned result = 1;
    result += CountLogicalOperators(stmt->getCond());
    Visit(stmt->getBody());
    result += count;
    count = result;
}

void StmtNPathVisitor::VisitCaseStmt(clang::CaseStmt *stmt)
{
    if(!stmt)
        count = 1;
    else
        Visit(stmt->getSubStmt());
}

void StmtNPathVisitor::VisitSwitchStmt(clang::SwitchStmt *stmt)
{
    long long unsigned result = 1;
    result += CountLogicalOperators(stmt->getCond());
    long long unsigned  case_range = 0;
    for(auto *c : stmt->getBody()->children())
    {
        if(c->getStmtClass() == Stmt::CaseStmtClass)
        {
            result += case_range;
            Visit(llvm::dyn_cast<CaseStmt>(c)->getSubStmt());
            case_range = count;
        }
        else
        {
            Visit(c);
            case_range *= count;
        }
    }
    result += case_range;
    count = result;
}

void StmtNPathVisitor::VisitIfStmt(clang::IfStmt *stmt)
{
    long long unsigned result = 1 - (stmt->getElse() != nullptr);
    result += CountLogicalOperators(stmt->getCond());
    Visit(stmt->getThen());
    result += count;

    if(decltype(stmt->getElse()) e = stmt->getElse())
    {
        Visit(e);
        result += count;
    }

    count = result;
}

void StmtNPathVisitor::VisitForStmt(clang::ForStmt *stmt)
{
    long long unsigned result = 1;
    result += CountLogicalOperators(stmt->getInit());
    result += CountLogicalOperators(stmt->getCond());
    result += CountLogicalOperators(stmt->getInc());
    Visit(stmt->getBody());
    result += count;
    count = result;
}

void StmtNPathVisitor::VisitCXXTryStmt(clang::CXXTryStmt *stmt)
{
    long long unsigned res = 0;
    if(!stmt)
    {
        count = 1;
        return;
    }

    Visit(stmt->getTryBlock());
    res += count;
    for(size_t i = 0; i < stmt->getNumHandlers(); ++ i)
    {
        Visit(llvm::dyn_cast_or_null<clang::CompoundStmt>(stmt->getHandler(i)->getHandlerBlock()));
        res += count;
    }
    count = res;
}

int StmtNPathVisitor::CountLogicalOperators(clang::Stmt *stmt)
{
    if(!stmt) return 0;
    ASTMatcherVisitor vis(ctx);
    Counter c;
    vis.AddMatchers({binaryOperator(hasOperatorName("&&")),
                     binaryOperator(hasOperatorName("and")),
                     binaryOperator(hasOperatorName("or")),
                     binaryOperator(hasOperatorName("||"))}, &c);
    vis.TraverseStmt(stmt);
    return c.getCount();
}

void StmtNPathVisitor::VisitReturnStmt(clang::ReturnStmt *stmt)
{
    count = CountLogicalOperators(stmt->getRetValue()) + 1;
}

void StmtNPathVisitor::VisitLambdaExpr(clang::LambdaExpr *expr)
{
    Visit(expr->getBody());
    lambda_count += count;
    count = 1;
}

void StmtNPathVisitor::VisitCXXForRangeStmt(clang::CXXForRangeStmt *stmt)
{
    Visit(stmt->getBody());
    count += 1;
}

void StmtNPathVisitor::Visit(clang::Stmt *stmt)
{
    if(!stmt)
        count = 1;
    else
        StmtVisitor<StmtNPathVisitor>::Visit(stmt);
}