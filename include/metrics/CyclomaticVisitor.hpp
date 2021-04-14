#ifndef METRICS_CYCLOMATICVISITOR_HPP
#define METRICS_CYCLOMATICVISITOR_HPP

#include <map>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "include/MetricVisitor.hpp"
#include "include/Utility.hpp"
#include "include/FunctionVisitor.hpp"

/**
 * Calculates cyclomatic complexity for given function.
 */
class CyclomaticVisitor : public FunctionVisitor
{
public:
    explicit CyclomaticVisitor(clang::ASTContext *context);

    virtual ~CyclomaticVisitor();

    virtual void CalcMetrics(clang::FunctionDecl *decl) override;
    int GetValue() const { return count; }

    virtual std::ostream &ExportXML(std::ostream &os) const override;
    virtual std::ostream &Export(std::ostream &os) const override;
protected:
    int count{1};
};


#endif //METRICS_CYCLOMATICVISITOR_HPP
