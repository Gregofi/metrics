
#ifndef METRICS_CYCLOMATICVISITOR_HPP
#define METRICS_CYCLOMATICVISITOR_HPP

#include <map>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "MetricVisitor.hpp"
#include "Metric.hpp"

class CyclomaticVisitor : public clang::RecursiveASTVisitor<CyclomaticVisitor>
{
public:
    explicit CyclomaticVisitor(clang::ASTContext *context) : context(context) {}
    bool VisitFunctionDecl(clang::FunctionDecl *decl);
    bool VisitIfStmt(clang::IfStmt *decl);

protected:
    clang::ASTContext *context;
};


#endif //METRICS_CYCLOMATICVISITOR_HPP
