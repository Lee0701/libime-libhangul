#include "SymbolTable.h"
#include "strcvt.h"

namespace HangulIME {
    SymbolTable::SymbolTable(const std::vector<std::string> &fileNames) {
        this->frequencies = new std::map<std::wstring, std::wstring>;
        char s[512];
        for(auto &fileName : fileNames) {
            FILE *fp = fopen(fileName.c_str(), "r");
            if(fp == nullptr) continue;

            while(fscanf(fp, "%s\n", s) > 0) {
                std::wstring str = s2ws(s);
                size_t index = str.find(L":");
                if(index == std::string::npos) continue;
                std::wstring key = str.substr(0, index);
                std::wstring value = str.substr(index + 1);
                this->frequencies->insert_or_assign(key, value);
            }

            fclose(fp);
        }
    }

    SymbolTable::~SymbolTable() {
        delete this->frequencies;
    }

    std::vector<std::wstring> SymbolTable::symbols(std::wstring &key) {
        std::vector<std::wstring> ret;
        if(this->frequencies->find(key) != this->frequencies->end()) {
            std::wstring str = this->frequencies->at(key);
            for(wchar_t c : str) {
                if(c == 0) continue;
                ret.push_back(std::wstring(1, c));
            }
        }
        return ret;
    }

}
