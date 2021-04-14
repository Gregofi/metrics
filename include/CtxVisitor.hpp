#ifndef METRICS_PROJECT_CTXVISITOR_HPP
#define METRICS_PROJECT_CTXVISITOR_HPP

#include "clang/AST/ASTContext.h"
#include <ostream>

/* This metric collect information about classes, but it needs to traverse whole unit, not just
 * one class because metric calculation needs to know about other classes.
 */
class CtxVisitor
{
public:
    explicit CtxVisitor(clang::ASTContext *ctx) : ctx(ctx) {}
    virtual ~CtxVisitor() = default;
    /**
     * Traverse UnitDecl and gather metrics.
     * @param decl - Translation unit for which metric will be calculated.
     */
    virtual void CalcMetrics(clang::TranslationUnitDecl *decl) = 0;

    /**
     * Export metrics to given stream, intended to be human readable.
     * @param id - ID of the class to export.
     * @param os - output stream to export to.
     * @return - given output stream.
     */
    virtual std::ostream& Export(size_t id, std::ostream &os) const = 0;

    /**
     * Export metric to given stream in XML format.
     * @param id - ID of the class to export.
     * @param os - output stream to export to.
     * @return - given output stream.
     */
    virtual std::ostream& ExportXML(size_t id, std::ostream &os) const = 0;
protected:
    clang::ASTContext *ctx;
};

#endif //METRICS_PROJECT_CTXVISITOR_HPP