#include "StringUtils.h"
#include <string>
#include <iostream>
#include <sstream>
#include <codecvt>
#include <locale>

std::string StringUtils::join(const std::list<std::wstring>& lst, const std::string& delim)
{
    std::string ret;
    for (const auto& s : lst) {
        if (!ret.empty())
            ret += delim;
        ret += ws2s(s);
    }
    return ret;
}

std::list<std::wstring> StringUtils::split(const std::wstring str, wchar_t delim) {
    std::list<std::wstring> parts;
    std::wstringstream wss(str);
    std::wstring temp;
    while (std::getline(wss, temp, delim)) {
        parts.push_back(temp);
    }
    return parts;
}

std::string StringUtils::ws2s(std::wstring in) {
    std::wstring string_to_convert;

    //setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.to_bytes(in);
}


std::wstring StringUtils::s2ws(std::string in) {
    static std::wstring_convert< std::codecvt<wchar_t, char, std::mbstate_t> > s2wsConverter;
    return s2wsConverter.from_bytes(in);
}