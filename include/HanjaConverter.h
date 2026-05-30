#pragma once
#include <vector>
#include <filesystem>
#include <hangul.h>
#include "HanjaFrequencyTable.h"

namespace HangulIME {
    class HanjaConverter {
    private:
        HanjaTable *htbl;
        HanjaFrequencyTable *hfreq;

    public:
        HanjaConverter(std::filesystem::path hanjaDir);
        ~HanjaConverter();

        std::vector<std::wstring> convert(const std::wstring &input);
    };
}
