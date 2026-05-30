#pragma once
#include <string>
#include <vector>
#include <map>

namespace HangulIME {
    class HanjaFrequencyTable {
    private:
        std::map<std::wstring, int> *frequencies;

    public:
        HanjaFrequencyTable(const std::vector<std::string> &fileNames);
        ~HanjaFrequencyTable();

        int frequency(std::wstring &key);
    };
}
