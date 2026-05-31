#pragma once
#include "InputMode.h"
#include <hangul.h>
#include "HangulIMESettings.h"
#include "InputContext.h"
#include "CandidateState.h"
#include "HanjaConverter.h"
#include "SymbolTable.h"

namespace HangulIME {
    class ManualConversionInputMode : public InputMode {
    private:
        HangulInputContext *hic;
        HanjaConverter *converter;
        SymbolTable *symbolTable;
        std::wstring composing;
        CandidateState *candidates;

    public:
        ManualConversionInputMode(HangulIMESettings *settings);
        ~ManualConversionInputMode();

        virtual void onActivate();
        virtual void onDeactivate();
        virtual bool testEditKey(int code);
        virtual bool onEditKey(void *context, int code);
        virtual bool onChar(void *context, int code);
        virtual void onReset(void *context);

    private:
        bool showCandidates(InputContext *context);
        bool commitCandidates(InputContext *context);

        void compose(InputContext *context, std::wstring *str);
        void commit(InputContext *context, std::wstring *str);
        void flush(InputContext *context);
        std::wstring commit_str();
        std::wstring preedit_str();
    };
}
