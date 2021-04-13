
#ifndef METRICS_METRIC_HPP
#define METRICS_METRIC_HPP

#include <sstream>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

/**
 * Simple callback class for matchers that counts how many times a match has been found.
 */
class Counter : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override
    {
        count += 1;
    }
    int getCount() const { return count; }
private:
    int count = 0;
};

template <typename T>
std::string Tag(const std::string &tag, const T &inside, bool newline = true)
{
    std::ostringstream oss;
    oss << "<" << tag << ">" << inside << "</" << tag << ">";
    if(newline)
        oss << "\n";
    return oss.str();
}

std::string Escape(const std::string &text)
{
    std::string result;
    for(char c: text)
    {
        if(c == '&')
            result += "&amp;";
        else if(c == '<')
            result += "&lt;";
        else if(c == '>')
            result += "&gt;";
        else if(c == '"')
            result += "&quot;";
        else if(c == '\'')
            result += "&apos;";
        else
            result += c;
    }
    return result;
}

#endif //METRICS_METRIC_HPP
