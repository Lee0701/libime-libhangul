#include "HanjaConverter.h"
#include <thread>
#include "strcvt.h"

namespace HangulIME {
    HanjaConverter::HanjaConverter(std::filesystem::path hanjaDir) {
        this->hanjaDir = hanjaDir;
        this->htbl = nullptr;
        this->hfreq = nullptr;
        std::thread loader(&HanjaConverter::loadTables, this);
        loader.detach();
    }

    HanjaConverter::~HanjaConverter() {
        if(this->htbl != nullptr) hanja_table_delete(htbl);
        if(this->hfreq != nullptr) delete this->hfreq;
    }

    void HanjaConverter::loadTables() {
        std::filesystem::path hanjaTablePath = hanjaDir / "hanja.txt";
        this->htbl = hanja_table_load(hanjaTablePath.string().c_str());
        std::vector<std::string> tablePaths;
        tablePaths.push_back((hanjaDir / "freq-hanja.txt").string());
        tablePaths.push_back((hanjaDir / "freq-hanjaeo.txt").string());
        this->hfreq = new HanjaFrequencyTable(tablePaths);
    }

    std::vector<std::wstring> HanjaConverter::convert(const std::wstring &input) {
        std::vector<std::wstring> result;
        if(this->htbl == nullptr || this->hfreq == nullptr) return result;

        std::string str = ws2s(input);
        HanjaList *list = hanja_table_match_exact(htbl, str.c_str());
        int len = hanja_list_get_size(list);
        for(int k = 0 ; k < len ; k++) {
            std::wstring cand = s2ws(hanja_list_get_nth_value(list, k));
            result.push_back(cand.substr(0, input.length()));
        }
        hanja_list_delete(list);
        std::sort(result.begin(), result.end(), [this] (std::wstring &a, std::wstring &b) -> bool {
            int freqA = hfreq->frequency(a);
            int freqB = hfreq->frequency(b);
            if(freqA != freqB) return freqA > freqB;
            return a < b;
        });
        return result;
    }
}
