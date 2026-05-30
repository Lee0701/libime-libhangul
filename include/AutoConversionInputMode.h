#pragma once
#include "InputMode.h"
#include <hangul.h>
#include <vector>
#include <filesystem>
#include "HangulIMESettings.h"
#include "InputContext.h"
#include "CandidateState.h"
#include "HanjaConverter.h"

namespace HangulIME {
    class AutoConversionInputMode : public InputMode {
    private:
        HangulInputContext *hic;
        HanjaConverter *converter;
        std::vector<int> inputs;
        std::wstring composing;
        std::wstring converted;
        std::wstring locked;
        CandidateState *candidates;

    public:
        AutoConversionInputMode(HangulIMESettings *settings);
        ~AutoConversionInputMode();

        virtual void onActivate();
        virtual void onDeactivate();
        virtual bool testEditKey(int code);
        virtual bool onEditKey(void *context, int code);
        virtual bool onChar(void *context, int code);
        virtual void onReset(void *context);

    private:
        void update(InputContext *context);
        void updateCandidates();
        void updateComposing();
        void updateConverted();

        std::wstring combineHangul();

        void compose(InputContext *context, std::wstring *str);
        void commit(InputContext *context, std::wstring *str);
        void flush(InputContext *context);
        void commitCurrent(InputContext *context);
    };
}
