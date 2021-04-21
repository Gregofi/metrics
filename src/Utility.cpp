#include "include/Utility.hpp"

std::string EscapeXML(const std::string &text)
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

std::string GetFunctionHead(const clang::FunctionDecl *decl)
{
    std::string res = decl->getQualifiedNameAsString() + "(";
    for(size_t i = 0; i < decl->getNumParams(); ++ i)
    {
        res += decl->getParamDecl(i)->getType().getAsString();
        if(i + 1 != decl->getNumParams())
            res += ", ";
    }
    res += ")";
    return res;
}
