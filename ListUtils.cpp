#include "ListUtils.h"

std::list<std::wstring> ListUtils::filterList(std::list<std::wstring> in, std::list<std::wstring> filter) {
    std::list<std::wstring> out;
    for (auto e : in) {
        if (!isInList(filter, e)) {
            out.push_back(e);
        }
    }
    return out;
}

bool ListUtils::isInList(std::list<std::wstring> list, std::wstring toFind) {
    for (auto e : list) {
        if (e.find(toFind) != std::string::npos) {
            return true;
        }
    }
    return false;
}