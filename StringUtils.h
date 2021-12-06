#pragma once
#include <string>
#include <list>

class StringUtils
{
public:
	static std::string join(const std::list<std::wstring>& lst, const std::string& delim);
	static std::list<std::wstring> split(const std::wstring str, wchar_t delim);
	static std::string ws2s(std::wstring in);
	static std::wstring s2ws(std::string in);
};

