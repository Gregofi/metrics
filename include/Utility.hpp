
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

/**
 * Intended for XML exports. Creates a tag with tagname 'tag' and with 'inside' content.
 * @tparam T - Type of the tag content
 * @param tag - Name of the tag
 * @param inside - Content of the tag, needs to have overloaded stream << operator
 * @param newline - If newline should be put after the end of tag.
 * @return - String containing the tag text.
 *
 * @example for call Tag("employee", Tag("pay", 1, false)) it returns "<employee><pay>1</pay></employee>"
 */
template <typename T>
std::string Tag(const std::string &tag, const T &inside, bool newline = true)
{
    std::ostringstream oss;
    oss << "<" << tag << ">" << inside << "</" << tag << ">";
    if(newline)
        oss << "\n";
    return oss.str();
}

/**
 * Escapes XML text, does following replacements:
 *  & -> &amp;
 *  < -> &lt;
 *  > -> &gt;
 *  " -> &quot;
 *  ' -> &apos;
 * including the semicolons.
 * @param text - Text to be escaped.
 * @return Escaped string
 */
std::string EscapeXML(const std::string &text);

/**
 * Returns function declaration in string, contains function name, parameters, constness and its ref-qualifiers.
 * @example For function int foo(int i, const std::string &s) returns "foo(int, const std::string &)"
 * @param decl
 * @return
 */
std::string GetFunctionHead(const clang::FunctionDecl *decl);
#endif //METRICS_METRIC_HPP
