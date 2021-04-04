#ifndef METRICS_PROJECT_FANSVISITOR_HPP
#define METRICS_PROJECT_FANSVISITOR_HPP

#include "include/FunctionVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/FunctionCtxVisitor.hpp"

class FanCount : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    void SetCurrFuncId(size_t id) { curr_func = id; InitFunction(id); }
    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
    void InitFunction(size_t id);
    const std::map<size_t, int>& GetFanIn()  const { return fan_in;  }
    const std::map<size_t, int>& GetFanOut() const { return fan_out; }
protected:
    size_t curr_func{};
    /** Number of functions, which call function with id in key.  */
    std::map<size_t, int> fan_out;
    /** Number of functions, which function with id in key calls.  */
    std::map<size_t, int> fan_in;
};

/**
 * Calculates the Fan-In and Fan-Out metric for each function. Fan-in is how many functions
 * has this function called and Fan-Out is how many times this function has been called.
 *
 * This Visitor needs to visit whole scope of calculated functions, not only one.
 * Often times you want it to visit whole TranslationUnitDecl.
 */
class FansVisitor : public clang::RecursiveASTVisitor<FansVisitor>, public FunctionCtxVisitor
{
public:
    explicit FansVisitor(clang::ASTContext *ctx);
    bool VisitFunctionDecl(clang::FunctionDecl *decl);
    bool VisitCXXMethodDecl(clang::CXXMethodDecl *decl);
    int FanIn(size_t id)  const { return counter.GetFanIn().at(id); }
    int FanOut(size_t id) const { return counter.GetFanOut().at(id); }

    virtual void CalcMetrics(clang::TranslationUnitDecl *decl) override;
    virtual std::ostream &Export(size_t id, std::ostream &os) const override;
protected:
    ASTMatcherVisitor vis;
    FanCount counter;
};

#endif //METRICS_PROJECT_FANSVISITOR_HPP
