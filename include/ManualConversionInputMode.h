#pragma once
#include <hangul.h>
#include "InputMode.h"
#include "InputContext.h"

namespace HangulIME {
    class ManualConversionInputMode : public InputMode {
    private:
        HangulInputContext *hic;

        std::wstring composing;

    public:
        ManualConversionInputMode(char *keyboardType);
        ~ManualConversionInputMode();

        virtual void onActivate();
        virtual void onDeactivate();
        virtual bool testEditKey(int code);
        virtual bool onEditKey(void *context, int code);
        virtual bool onChar(void *context, int code);
        virtual void onReset(void *context);

    private:
        void compose(InputContext *context, std::wstring *str);
        void commit(InputContext *context, std::wstring *str);
        void flush(InputContext *context);
        std::wstring commit_str();
        std::wstring preedit_str();
    };
}
