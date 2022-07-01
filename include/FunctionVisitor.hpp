#pragma once

#include <ostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "include/Utility.hpp"

/**
 * This class represents an interface for visitors that only examine one function at a time.
 * CalcMetric should be called to run calculations. Export to export calculated metrics.
 */
class FunctionVisitor {
public:
    explicit FunctionVisitor(clang::ASTContext* ctx)
        : context(ctx)
    {
    }

    virtual ~FunctionVisitor() = default;

    /**
     * Export metric, intended to be human readable.
     * @param os - output stream to export to.
     * @return - given output stream.
     */
    virtual std::ostream& Export(std::ostream& os   ) const = 0;

    /**
     * Export metric in XML format with enclosing tags.
     * @param os - output stream to export to.
     * @return - given output stream
     */
    virtual std::ostream& ExportXML(std::ostream& os) const = 0;

    /**
     * Calculate metrics.
     * @param decl - Declaration for which metrics should be calculated, this should typically be function declaration.
     */
    virtual void CalcMetrics(clang::FunctionDecl* decl) = 0;

protected:
    clang::ASTContext* context;
};
