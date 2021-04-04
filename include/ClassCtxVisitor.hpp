#ifndef METRICS_PROJECT_CLASSCTXVISITOR_HPP
#define METRICS_PROJECT_CLASSCTXVISITOR_HPP

#include "clang/AST/ASTContext.h"
#include <ostream>

/* This metric collect information about classes, but it needs to traverse whole unit, not just
 * one class because it's metrics need to know about other classes.
 */
class ClassCtxVisitor
{
public:
    explicit ClassCtxVisitor(clang::ASTContext *ctx) : ctx(ctx) {}
    virtual ~ClassCtxVisitor() = default;
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

#endif //METRICS_PROJECT_CLASSCTXVISITOR_HPP
