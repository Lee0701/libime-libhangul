#include "InputContext.h"

namespace HangulIME {
    void InputContext::compose(std::wstring *text) {
        if(text == nullptr) return;
        service->setCompositionString(context, text->c_str(), (int) text->length());
    }

    void InputContext::commit(std::wstring *text) {
        if(text == nullptr || text->length() == 0) return;
        if(service->isComposing()) {
            service->setCompositionString(context, text->c_str(), (int) text->length());
            service->endComposition(context);
        } else {
            service->appendText(context, text->c_str(), (int) text->length());
        }
    }

    void InputContext::updateComposingWindow(std::wstring *composing) {
        if(composing == nullptr || composing->length() == 0) {
            service->hideComposingWindow();
        } else {
            service->updateComposingWindow(context, composing);
        }
    }

    void InputContext::updateCandidateWindow(std::vector<std::wstring> *candidates) {
        if(candidates == nullptr || candidates->size() == 0) {
            service->hideCandidateWindow();
        } else {
            service->updateCandidateWindow(context, candidates);
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
