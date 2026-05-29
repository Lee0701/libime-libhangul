#pragma once
#include "InputMode.h"

namespace HangulIME {
    class AutoConversionInputMode : public InputMode {
    public:
        AutoConversionInputMode();
        ~AutoConversionInputMode();

        virtual void onActivate();
        virtual void onDeactivate();
        virtual bool testEditKey(int code);
        virtual bool onEditKey(void *context, int code);
        virtual void onChar(void *context, int code);
        virtual void onReset(void *context);
    };
}
