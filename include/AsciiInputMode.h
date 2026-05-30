#pragma once
#include "InputMode.h"

namespace HangulIME {
    class AsciiInputMode : public InputMode {
    public:
        AsciiInputMode();
        virtual ~AsciiInputMode();

        virtual void onActivate();
        virtual void onDeactivate();
        virtual bool testEditKey(int code);
        virtual bool onEditKey(void *context, int code);
        virtual bool onChar(void *context, int code);
        virtual void onReset(void *context);
    };
}
