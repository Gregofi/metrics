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

    /* Handle ref qualifiers such as 'int a() const &&'
     * Only methods can have those, so we cast it first. */
    if(auto method = llvm::dyn_cast_or_null<clang::CXXMethodDecl>(decl); method)
    {
        res += " ";
        if(method->isConst())
        {
            res += "const ";
        }
        switch(method->getRefQualifier())
        {
            case clang::RefQualifierKind::RQ_LValue:
                res += "&";
                break;
            case clang::RefQualifierKind::RQ_RValue:
                res += "&&";
                break;
            case clang::RefQualifierKind::RQ_None:
                break;
        }
        if(res[res.length() - 1] == ' ')
            res.pop_back();
    }

    return res;
}
