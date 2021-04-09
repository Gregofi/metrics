#include "include/metrics/NPathVisitor.hpp"
#include "include/Logging.hpp"
#include "include/ASTMatcherVisitor.hpp"

using namespace clang;
using namespace clang::ast_matchers;

NPathVisitor::NPathVisitor(clang::ASTContext *ctx) : FunctionVisitor(ctx)
{

}

void NPathVisitor::CalcMetrics(clang::Decl *decl)
{
    this->TraverseDecl(decl);
}

bool NPathVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
    auto *body = llvm::dyn_cast<CompoundStmt>(decl->getBody());
    StmtNPathVisitor visitor(context);
    visitor.Visit(body);
    count = visitor.GetCount();
    return true;
}

std::ostream &NPathVisitor::Export(std::ostream &os) const
{
    os << "NPATH: " << count << "\n";
    return os;
}

/*-----------------------------------------------------------------------------------*/

void StmtNPathVisitor::VisitStmt(Stmt *stmt)
{
    LOG("Visiting statement: " << stmt->getStmtClassName());
    int result = 1;
    for(const auto & x : stmt->children())
    {
        Visit(x);
        result += count;
    }
    count = result;
}

void StmtNPathVisitor::VisitCompoundStmt(clang::CompoundStmt *stmt)
{
    int result = 1;
    for(const auto &x : stmt->children())
    {
        Visit(x);
        /* TODO: Unfortunely, some clang statements are expressions(for example 'a += 3' is expr),
         * and expressions returns number of logic operators. That would mess up our counting. */
        result *= count != 0 ? count : 1;
    }
    count = result;
}

void StmtNPathVisitor::VisitExpr(clang::Expr *stmt)
{
    int result = 0;
    for(const auto & x : stmt->children())
    {
        Visit(x);
        result += count;
    }
    count = result;
}

void StmtNPathVisitor::VisitDoStmt(clang::DoStmt *stmt)
{
    int result = 1;
    result += CountLogicalOperators(stmt->getCond());
    Visit(stmt->getBody());
    result += count;
    count = result;
}

void StmtNPathVisitor::VisitWhileStmt(clang::WhileStmt *stmt)
{
    int result = 1;
    result += CountLogicalOperators(stmt->getCond());
    Visit(stmt->getBody());
    result += count;
    count = result;
}

void StmtNPathVisitor::VisitSwitchStmt(clang::SwitchStmt *stmt)
{
    int result = 1;
    result += CountLogicalOperators(stmt->getCond());
    for(clang::SwitchCase *c = stmt->getSwitchCaseList(); c != nullptr; c = c->getNextSwitchCase() )
    {
        Visit(c);
        result += count;
    }
    result += 1;
}

void StmtNPathVisitor::VisitIfStmt(clang::IfStmt *stmt)
{
    int result = 1 - (stmt->getElse() != nullptr);
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
    int result = 1;
    result += CountLogicalOperators(stmt->getInit());
    result += CountLogicalOperators(stmt->getCond());
    result += CountLogicalOperators(stmt->getInc());
    Visit(stmt->getBody());
    result += count;
    count = result;
}

void StmtNPathVisitor::VisitCXXCatchStmt(clang::CXXCatchStmt *stmt)
{

}

void StmtNPathVisitor::VisitCXXTryStmt(clang::CXXTryStmt *stmt)
{

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
    LOG("Number of operators: " << count);
}
