#pragma once
#include <vector>
#include <filesystem>
#include <hangul.h>
#include "HanjaFrequencyTable.h"

namespace HangulIME {
    class HanjaConverter {
    private:
        std::filesystem::path hanjaDir;
        HanjaTable *htbl;
        HanjaFrequencyTable *hfreq;

    public:
        HanjaConverter(std::filesystem::path hanjaDir);
        ~HanjaConverter();

        void loadTables();
        std::vector<std::wstring> convert(const std::wstring &input);
    };
}
