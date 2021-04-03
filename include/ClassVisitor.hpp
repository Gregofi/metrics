#ifndef METRICS_PROJECT_CLASSVISITOR_HPP
#define METRICS_PROJECT_CLASSVISITOR_HPP

#include "include/Metric.hpp"

class ClassVisitor
{
public:
    explicit ClassVisitor(clang::ASTContext *ctx) : context(ctx) {}
    virtual ~ClassVisitor() = default;
    virtual void CalcMetrics(clang::CXXRecordDecl *decl) = 0;
protected:
    clang::ASTContext *context;
};

#endif //METRICS_PROJECT_CLASSVISITOR_HPP
