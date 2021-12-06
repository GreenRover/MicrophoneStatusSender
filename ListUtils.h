#pragma once
#include <string>
#include <list>

class ListUtils
{
public:
	static std::list<std::wstring> filterList(std::list<std::wstring> in, std::list<std::wstring> filter);
	static bool isInList(std::list<std::wstring> list, std::wstring toFind);
};

