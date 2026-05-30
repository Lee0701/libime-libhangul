#pragma once

namespace HangulIME {
    class InputMode {
    public:
        InputMode();
        virtual ~InputMode();

        virtual void onActivate();
        virtual void onDeactivate();
        virtual bool testEditKey(int code);
        virtual bool onEditKey(void *context, int code);
        virtual bool onChar(void *context, int code);
        virtual void onReset(void *context);
    };
}
