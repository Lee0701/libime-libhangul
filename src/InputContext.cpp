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

    void InputContext::updateComposingWindow(std::wstring *composing) {
        if(composing == nullptr || composing->length() == 0) {
            service->hideComposingWindow();
        } else {
            service->updateComposingWindow(session, composing);
        }
    }

    void InputContext::updateCandidateWindow(std::vector<std::wstring> *candidates) {
        if(candidates == nullptr || candidates->size() == 0) {
            service->hideCandidateWindow();
        } else {
            service->updateCandidateWindow(session, candidates);
        }
    }

    int InputContext::getSelectedIndex() {
        auto window = service->getCandidateWindow();
        int selection = -1;
        if(window == nullptr) return selection;
        if(window->GetSelection((UINT *) &selection) != S_OK) return selection;
        return selection;
    }

    void InputContext::setSelectedIndex(int index) {
        if(index < 0) return;
        auto window = service->getCandidateWindow();
        if(window == nullptr) return;
        window->setCurrentSel(index);
    }

    void InputContext::setPage(int size, int index) {
        auto window = service->getCandidateWindow();
        if(window == nullptr) return;
        window->SetPageIndex((UINT *) &index, (UINT) size);
    }
}
