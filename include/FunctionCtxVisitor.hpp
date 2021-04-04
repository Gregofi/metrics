#ifndef METRICS_PROJECT_FUNCTIONCTXVISITOR_HPP
#define METRICS_PROJECT_FUNCTIONCTXVISITOR_HPP

#include "clang/AST/ASTContext.h"
#include <ostream>

/**
 * Represents class that gathers metrics about functions, but needs to know about
 * other function as well, because metric calculation depends on it.
 */
class FunctionCtxVisitor
{
public:
    FunctionCtxVisitor(clang::ASTContext *ctx) : ctx(ctx) {}
    virtual ~FunctionCtxVisitor() = default;
    /**
     * Calculates metric about functions for whole decl.
     */
    virtual void CalcMetrics(clang::TranslationUnitDecl *decl) = 0;
    /**
     * Exports metrics to stdout.
     * @param id - Id of the function to export
     */
    virtual std::ostream& Export(size_t id, std::ostream &os) const = 0;
protected:
    clang::ASTContext *ctx;
};

#endif //METRICS_PROJECT_FUNCTIONCTXVISITOR_HPP
