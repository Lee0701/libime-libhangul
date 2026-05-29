#include "InputContext.h"

namespace HangulIME {
    void InputContext::compose(std::wstring *text) {
        if(!service->isComposing()) service->startComposition(session->context());
        service->setCompositionString(session, text->c_str(), (int) text->length());
    }

    void InputContext::commit(std::wstring *text) {
        if(!service->isComposing()) service->startComposition(session->context());
        service->setCompositionString(session, text->c_str(), (int) text->length());
        service->endComposition(session->context());
    }
}
