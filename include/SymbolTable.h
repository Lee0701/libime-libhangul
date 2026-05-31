#pragma once
#include <string>
#include <vector>
#include <map>

namespace HangulIME {
    class SymbolTable {
    private:
        std::map<std::wstring, std::wstring> *frequencies;

    public:
        SymbolTable(const std::vector<std::string> &fileNames);
        ~SymbolTable();

        std::vector<std::wstring> symbols(std::wstring &key);
    };
}
