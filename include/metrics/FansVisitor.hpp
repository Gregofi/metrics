#ifndef METRICS_PROJECT_FANSVISITOR_HPP
#define METRICS_PROJECT_FANSVISITOR_HPP

#include "include/FunctionVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"
#include "include/CtxVisitor.hpp"

/**
 * Callback for counting fans.
 * @detail Implementation is not ideal. Before it is run, it needs to know
 * about which function it visits. Also, after it is done visiting it needs to know that
 * its not in a function anymore. So the order should be
 *   - SetCurrFuncId(current_function_id);
 *   - Visit(decl); // Visitor that tries to match the function body
 *   - LeaveFunction();
 */
class FanCount : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    /**
     * Fan-in will be counted towards function with id in param.
     * @param id
     */
    void SetCurrFuncId(const std::string &s) { curr_func = s; InitFunction(s); is_in_func = true; }
    /**
     * Callback function for matchers.
     * @param Result
     */
    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
    void LeaveFunction() {is_in_func = false;}
    const std::map<std::string, int>& GetFanIn()  const { return fan_in;  }
    const std::map<std::string, int>& GetFanOut() const { return fan_out; }
protected:
    void InitFunction(const std::string &s);
    std::string curr_func;
    bool is_in_func;
    /** Number of functions, which call function with id in key.  */
    std::map<std::string, int> fan_out;
    /** Number of functions, which function with id in key calls.  */
    std::map<std::string, int> fan_in;
};

/**
 * Calculates the Fan-In and Fan-Out metric for each function. Fan-in is how many functions
 * has this function called and Fan-Out is how many times this function has been called.
 *
 * This Visitor needs to visit whole scope of calculated functions, not only one.
 * Most times you want it to visit whole TranslationUnitDecl.
 */
class FansVisitor : public clang::RecursiveASTVisitor<FansVisitor>, public CtxVisitor
{
public:
    bool VisitFunctionDecl(clang::FunctionDecl *d);
    /**
     * Returns fan-in value for given functionID
     * @throws - std::out_of_range if metrics haven't been calculated for given classID (Either CalcMetrics wasn't called
     *              or class with given ID doesn't exist).
     */
    int FanIn(const std::string &s)  const { return counter.GetFanIn().at(s); }
    /**
     * Returns fan-out value for given functionID
     * @throws - std::out_of_range if metrics haven't been calculated for given classID (Either CalcMetrics wasn't called
     *              or class with given ID doesn't exist).
     */
    int FanOut(const std::string &s) const { return counter.GetFanOut().at(s); }

    virtual void CalcMetrics(clang::ASTContext *ctx) override;
    /**
     * All lambdas are skipped, because they are already traversed by the matchers.
     * @param expr
     * @return
     */
    bool TraverseLambdaExpr(clang::LambdaExpr *expr);
    virtual std::ostream &Export(const std::string &s, std::ostream &os) const override;
    virtual std::ostream &ExportXML(const std::string &s, std::ostream &os) const override;
protected:
    FanCount counter;
};

#endif //METRICS_PROJECT_FANSVISITOR_HPP
