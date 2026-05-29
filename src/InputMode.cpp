#include "InputMode.h"

namespace HangulIME {
    InputMode::InputMode() {
    }

    InputMode::~InputMode() {
    }

    void InputMode::onActivate() {
    }

    void InputMode::onDeactivate() {
    }

    bool InputMode::testEditKey(int code) {
        return false;
    }

    bool InputMode::onEditKey(void *context, int code) {
        return false;
    }

    void InputMode::onChar(void *context, int code) {
    }

    void InputMode::onReset(void *context) {
    }
}
