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