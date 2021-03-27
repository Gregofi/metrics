
#ifndef METRICS_CYCLOMATICVISITOR_HPP
#define METRICS_CYCLOMATICVISITOR_HPP

#include <map>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "include/MetricVisitor.hpp"
#include "include/Metric.hpp"
#include "include/AbstractVisitor.hpp"

/**
 *
 */
class CyclomaticVisitor : public AbstractVisitor, public clang::RecursiveASTVisitor<CyclomaticVisitor>
{
public:
    explicit CyclomaticVisitor(clang::ASTContext *context);

    virtual ~CyclomaticVisitor();
    bool VisitFunctionDecl(clang::FunctionDecl *decl);

    virtual void CalcMetrics(clang::Decl *decl) override
    {
        this->TraverseDecl(decl);
    }

protected:
    size_t count = 0;
};


#endif //METRICS_CYCLOMATICVISITOR_HPP
