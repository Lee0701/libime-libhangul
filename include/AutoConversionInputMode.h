#pragma once
#include "InputMode.h"
#include <hangul.h>
#include <vector>
#include <filesystem>
#include "InputContext.h"
#include "CandidateState.h"

namespace HangulIME {
    class AutoConversionInputMode : public InputMode {
    private:
        HangulInputContext *hic;
        HanjaTable *htbl;
        std::vector<int> inputs;
        std::wstring composing;
        CandidateState *candidates;

    public:
        AutoConversionInputMode(std::filesystem::path installDir, char *keyboardType);
        ~AutoConversionInputMode();

        virtual void onActivate();
        virtual void onDeactivate();
        virtual bool testEditKey(int code);
        virtual bool onEditKey(void *context, int code);
        virtual void onChar(void *context, int code);
        virtual void onReset(void *context);

    private:
        void update(InputContext *context);
        void updateCandidates();
        void updateComposing();

        std::wstring combineHangul();

        void compose(InputContext *context, std::wstring *str);
        void commit(InputContext *context, std::wstring *str);
        void flush(InputContext *context);
    };
}
