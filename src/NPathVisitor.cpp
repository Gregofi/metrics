//
// Created by filip on 3/26/21.
//

#include "include/NPathVisitor.hpp"
#include "include/Logging.hpp"

using namespace clang;

NPathVisitor::NPathVisitor(clang::ASTContext *ctx) : AbstractVisitor(ctx)
{

}

void NPathVisitor::CalcMetrics(clang::Decl *decl)
{
    this->TraverseDecl(decl);
}

bool NPathVisitor::VisitFunctionDecl(clang::FunctionDecl *decl)
{
    auto *body = llvm::dyn_cast<CompoundStmt>(decl->getBody());
    StmtNPathVisitor visitor;
    visitor.Visit(body);
    metrics.push_back({"NPATH", visitor.GetCount()});
    return true;
}

/*-----------------------------------------------------------------------------------*/

void StmtNPathVisitor::VisitStmt(const Stmt *stmt)
{
    LOG("Visiting statement: " << stmt->getStmtClassName());
    int result = 1;
    for(const auto & x : stmt->children())
    {
        Visit(x);
        result *= count;
    }
    count = result;
}

void StmtNPathVisitor::VisitCompoundStmt(const clang::CompoundStmt *stmt)
{
    int result = 1;
    for(const auto &x : stmt->children())
    {
        Visit(x);
        result *= count;
    }
    count = result;
}

void StmtNPathVisitor::VisitExpr(const clang::Expr *stmt)
{
    LOG("Visiting expr: " << stmt->getStmtClassName());
    int result = 0;
    for(const auto & x : stmt->children())
    {
        Visit(x);
        result *= count;
    }
    count = result;
}

void StmtNPathVisitor::VisitDoStmt(const clang::DoStmt *stmt)
{
    int result = 1;
    Visit(stmt->getCond());
    result += count;
    Visit(stmt->getBody());
    result += count;
    count = result;
}

void StmtNPathVisitor::VisitWhileStmt(const clang::WhileStmt *stmt)
{
    int result = 1;
    Visit(stmt->getCond());
    result += count;
    Visit(stmt->getBody());
    result += count;
    count = result;
}

void StmtNPathVisitor::VisitSwitchStmt(const clang::SwitchStmt *stmt)
{
    int result = 1;
    Visit(stmt->getCond());
    for(const clang::SwitchCase *c = stmt->getSwitchCaseList(); c != nullptr; c = c->getNextSwitchCase() )
    {
        Visit(c);
        result += count;
    }
    result += 1;
}

void StmtNPathVisitor::VisitIfStmt(const clang::IfStmt *stmt)
{
    LOG("Visiting statement: " << stmt->getStmtClassName());
    int result = 1 - (stmt->getElse() != nullptr);
    Visit(stmt->getCond());
    result += count;
    LOG("Visited expr: " << count);
    Visit(stmt->getThen());
    result += count;
    LOG("Visited then: " << count);

    if(decltype(stmt->getElse()) e = stmt->getElse())
    {
        Visit(e);
        result += count;
        LOG("Visited else: " << count);
    }

    count = result;
}

void StmtNPathVisitor::VisitCatch(const clang::CXXCatchStmt *stmt)
{
    int result = 1;
}

void StmtNPathVisitor::VisitTry(const clang::CXXTryStmt *stmt)
{
    int result = 1;
}



