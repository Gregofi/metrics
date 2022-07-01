#pragma once

#include <map>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "include/FunctionVisitor.hpp"
#include "include/MetricVisitor.hpp"
#include "include/Utility.hpp"

/**
 * Calculates cyclomatic complexity for given function.
 */
class CyclomaticVisitor : public FunctionVisitor {
public:
    explicit CyclomaticVisitor(clang::ASTContext* context);

    virtual ~CyclomaticVisitor();

    /**
     * Calculates cyclomatic complexity for given function
     * @param decl
     */
    virtual void CalcMetrics(clang::FunctionDecl* decl) override;

    /**
     * Returns counted complexity
     * @return
     */
    int GetValue() const { return count; }

    virtual std::ostream& ExportXML(std::ostream& os) const override;
    virtual std::ostream& Export(std::ostream& os) const override;

protected:
    int count { 1 };
};
