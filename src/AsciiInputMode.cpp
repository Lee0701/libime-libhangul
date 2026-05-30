#include "AsciiInputMode.h"
#include "InputContext.h"

namespace HangulIME {
    AsciiInputMode::AsciiInputMode() : InputMode() {
    }

    AsciiInputMode::~AsciiInputMode() {
    }

    void AsciiInputMode::onActivate() {
    }

    void AsciiInputMode::onDeactivate() {
    }

    bool AsciiInputMode::testEditKey(int code) {
        return false;
    }

    bool AsciiInputMode::onEditKey(void *context, int code) {
        return false;
    }

    bool AsciiInputMode::onChar(void *context, int code) {
        InputContext *ic = (InputContext *) context;
        std::wstring str(1, code);
        ic->commit(&str);
        return true;
    }

    void AsciiInputMode::onReset(void *context) {
    }
}
