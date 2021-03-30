#ifndef METRICS_PROJECT_FANSVISITOR_HPP
#define METRICS_PROJECT_FANSVISITOR_HPP

#include "include/FunctionVisitor.hpp"
#include "include/ASTMatcherVisitor.hpp"

class FanCount : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    void SetCurrFuncId(size_t id) { curr_func = id; InitFunction(id); }
    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override
    {
        if(const auto *s = Result.Nodes.getNodeAs<clang::CallExpr>("call"))
        {
            fan_in[curr_func] += 1;
            fan_out[s->getCalleeDecl()->getID()] += 1;
        }
        if(const auto *s = Result.Nodes.getNodeAs<clang::CXXOperatorCallExpr>("call"))
        {
            fan_in[curr_func] += 1;
            fan_out[s->getCalleeDecl()->getID()] += 1;
        }
        if(const auto *s = Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("call"))
        {
            fan_in[curr_func] += 1;
            fan_out[s->getCalleeDecl()->getID()] += 1;
        }
    }
    void InitFunction(size_t id)
    {
        if(!fan_out.count(id))
            fan_out[id];
        if(!fan_in.count(id))
            fan_in[id];
    }
    const std::map<size_t, int>& GetFanIn()  { return fan_in;  }
    const std::map<size_t, int>& GetFanOut() { return fan_out; }
protected:
    size_t curr_func{};
    /** Number of functions, which call function with id in key.  */
    std::map<size_t, int> fan_out;
    /** Number of functions, which function with id in key calls.  */
    std::map<size_t, int> fan_in;
};

class FansVisitor : public clang::RecursiveASTVisitor<FansVisitor>
{
public:
    explicit FansVisitor(clang::ASTContext *ctx);
    bool VisitFunctionDecl(clang::FunctionDecl *decl);
    bool VisitCXXMethodDecl(clang::CXXMethodDecl *decl);
    int FanIn(size_t id) { return counter.GetFanIn().at(id); }
    int FanOut(size_t id) { return counter.GetFanOut().at(id); }

protected:
    ASTMatcherVisitor vis;
    FanCount counter;
    clang::ASTContext *ctx;
};

#endif //METRICS_PROJECT_FANSVISITOR_HPP
