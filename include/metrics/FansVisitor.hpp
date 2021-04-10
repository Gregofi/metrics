#ifndef METRICS_PROJECT_FANSVISITOR_HPP
#define METRICS_PROJECT_FANSVISITOR_HPP

#include "include/FunctionVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/CtxVisitor.hpp"

/**
 * Callback for counting fans.
 * @detail Implementation is not ideal. Before it is run, it needs to know
 * about which function it visits. Also, after that it needs to know that
 * its not in function. So the order should be
 * SetCurrFuncId(current_function_id);
 * Visit(decl); // Visitor that tries to match the function body
 * LeaveFunction();
 */
class FanCount : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    /**
     * Fan-in will be counted towards function with id in param.
     * @param id
     */
    void SetCurrFuncId(size_t id) { curr_func = id; InitFunction(id); is_in_func = true; }
    /**
     * Callback function for matchers.
     * @param Result
     */
    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
    void LeaveFunction() {is_in_func = false;}
    const std::map<size_t, int>& GetFanIn()  const { return fan_in;  }
    const std::map<size_t, int>& GetFanOut() const { return fan_out; }
protected:
    /**
     *
     * @param id
     */
    void InitFunction(size_t id);
    size_t curr_func{};
    bool is_in_func;
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
class FansVisitor : public clang::RecursiveASTVisitor<FansVisitor>, public CtxVisitor
{
public:
    explicit FansVisitor(clang::ASTContext *ctx);
    bool VisitFunctionDecl(clang::FunctionDecl *decl);
    int FanIn(size_t id)  const { return counter.GetFanIn().at(id); }
    int FanOut(size_t id) const { return counter.GetFanOut().at(id); }

    virtual void CalcMetrics(clang::TranslationUnitDecl *decl) override;
    virtual std::ostream &Export(size_t id, std::ostream &os) const override;
protected:
    ASTMatcherVisitor vis;
    FanCount counter;
};

#endif //METRICS_PROJECT_FANSVISITOR_HPP
