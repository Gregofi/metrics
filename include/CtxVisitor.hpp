#ifndef METRICS_PROJECT_CTXVISITOR_HPP
#define METRICS_PROJECT_CTXVISITOR_HPP

#include "clang/AST/ASTContext.h"
#include <ostream>

/* This metric collect information about classes, but it needs to traverse whole unit, not just
 * one class because it's metrics need to know about other classes.
 */
class CtxVisitor
{
public:
    explicit CtxVisitor(clang::ASTContext *ctx) : ctx(ctx) {}
    virtual ~CtxVisitor() = default;
    /**
     * Traverse UnitDecl and gather metrics.
     * @param decl
     */
    virtual void CalcMetrics(clang::TranslationUnitDecl *decl) = 0;
    /**
     * Export metrics to stdout.
     * @param id - Id of the class to export
     */
    virtual std::ostream& Export(size_t id, std::ostream &os) const = 0;
protected:
    clang::ASTContext *ctx;
};

#endif //METRICS_PROJECT_CTXVISITOR_HPP
