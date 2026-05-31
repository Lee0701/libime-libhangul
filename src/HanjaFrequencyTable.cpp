#include "HanjaFrequencyTable.h"
#include "strcvt.h"

namespace HangulIME {
    HanjaFrequencyTable::HanjaFrequencyTable(const std::vector<std::string> &fileNames) {
        this->frequencies = new std::map<std::wstring, int>;
        char s[64];
        for(auto &fileName : fileNames) {
            FILE *fp = fopen(fileName.c_str(), "r");
            if(fp == nullptr) continue;

            while(fscanf(fp, "%s\n", s) > 0) {
                std::wstring str = s2ws(s);
                size_t index = str.find(L":");
                if(index == std::string::npos) continue;
                std::wstring key = str.substr(0, index);
                int freq = std::stoi(str.substr(index + 1).c_str());
                freq = freq % 1000000;
                if(this->frequencies->find(key) != this->frequencies->end()) {
                    int val = this->frequencies->at(key);
                    if(val > freq) freq = val;
                }
                this->frequencies->insert_or_assign(key, freq);
            }

            fclose(fp);
        }
    }

    HanjaFrequencyTable::~HanjaFrequencyTable() {
        delete this->frequencies;
    }

    int HanjaFrequencyTable::frequency(std::wstring &key) {
        return frequencies->count(key) > 0 ? frequencies->at(key) : 0;
    }
}
