#include "AutoConversionInputMode.h"

namespace HangulIME {
    AutoConversionInputMode::AutoConversionInputMode() : InputMode() {
    }

    AutoConversionInputMode::~AutoConversionInputMode() {
    }

    void AutoConversionInputMode::onActivate() {
    }

    void AutoConversionInputMode::onDeactivate() {
    }

    bool AutoConversionInputMode::testEditKey(int code) {
        return false;
    }

    bool AutoConversionInputMode::onEditKey(void *context, int code) {
        return false;
    }

    void AutoConversionInputMode::onChar(void *context, int code) {
    }

    void AutoConversionInputMode::onReset(void *context) {
    }
};
