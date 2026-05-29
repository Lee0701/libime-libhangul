#include "HangulIMETextService.h"
#include <filesystem>
#include "InputContext.h"
#include "ManualConversionInputMode.h"
#include "AutoConversionInputMode.h"

namespace HangulIME {
    TextService::TextService(ImeModule *module) : Ime::TextService(module) {
        // this->inputMode = new ManualConversionInputMode("2");
        char path[MAX_PATH];
        GetModuleFileName(module->hInstance(), path, sizeof(path));
        std::filesystem::path installDir = std::filesystem::path(path).parent_path();
        this->inputMode = new AutoConversionInputMode(installDir,  "2");

        LOGFONT lf;
        composingFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
        GetObject(composingFont, sizeof(lf), &lf);
        lf.lfHeight = fontHeight(14);
        lf.lfWeight = FW_NORMAL;
        composingFont = CreateFontIndirect(&lf);

        candidateFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
        GetObject(candidateFont, sizeof(lf), &lf);
        lf.lfHeight = fontHeight(14);
        lf.lfWeight = FW_NORMAL;
        candidateFont = CreateFontIndirect(&lf);
    }

    TextService::~TextService() {
        delete this->inputMode;
    }

    void TextService::onActivate() {
        inputMode->onActivate();
    }

    void TextService::onDeactivate() {
        inputMode->onDeactivate();
    }

    void TextService::onFocus() {
    }

    bool TextService::filterKeyDown(Ime::KeyEvent& keyEvent) {
        if(keyEvent.isKeyDown(VK_CONTROL) || keyEvent.isKeyDown(VK_LCONTROL) || keyEvent.isKeyDown(VK_RCONTROL)) {
            return true;
        }
        int keyCode = keyEvent.keyCode();
        switch(keyCode) {
        case VK_BACK:
        case VK_SPACE:
        case VK_RETURN:
        case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
            return inputMode->testEditKey(keyCode);
        }
        return keyEvent.isChar();
    }

    bool TextService::onKeyDown(Ime::KeyEvent& keyEvent, Ime::EditSession *session) {
        InputContext context(session, this);
        if(keyEvent.isKeyDown(VK_CONTROL) || keyEvent.isKeyDown(VK_LCONTROL) || keyEvent.isKeyDown(VK_RCONTROL)) {
            inputMode->onReset(&context);
            return false;
        }
        int keyCode = keyEvent.keyCode();
        switch(keyCode) {
        case VK_BACK:
        case VK_SPACE:
        case VK_RETURN:
        case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
            return inputMode->onEditKey(&context, keyCode);
        }
        inputMode->onChar(&context, keyEvent.charCode());
        return true;
    }

    bool TextService::filterKeyUp(Ime::KeyEvent& keyEvent) {
        return false;
    }

    bool TextService::onKeyUp(Ime::KeyEvent& keyEvent, Ime::EditSession *session) {
        return false;
    }

    bool TextService::onPreservedKey(const GUID& guid) {
        return false;
    }

    bool TextService::onCommand(UINT id, CommandType type) {
        return false;
    }

    // called when a compartment value is changed
    void TextService::onCompartmentChanged(const GUID& key) {
        Ime::TextService::onCompartmentChanged(key);
    }

    // called when the keyboard is opened or closed
    void TextService::onKeyboardStatusChanged(bool opened) {
        Ime::TextService::onKeyboardStatusChanged(opened);
    }

    // called just before current composition is terminated for doing cleanup.
    // if forced is true, the composition is terminated by others, such as
    // the input focus is grabbed by another application.
    // if forced is false, the composition is terminated gracefully by endComposition().
    void TextService::onCompositionTerminated(bool forced) {
    }

    void TextService::compose(Ime::EditSession *session, std::wstring *text) {
        if(!this->isComposing()) this->startComposition(session->context());
        this->setCompositionString(session, text->c_str(), (int) text->length());
    }

    void TextService::commit(Ime::EditSession *session, std::wstring *text) {
        if(!this->isComposing()) this->startComposition(session->context());
        this->setCompositionString(session, text->c_str(), (int) text->length());
        this->endComposition(session->context());
    }

    void TextService::createComposingWindow(Ime::EditSession *session) {
        if(!composingWindow) {
            composingWindow = new Ime::CandidateWindow(this, session);
            composingWindow->Release();

            composingWindow->setFont(composingFont);

            auto elementMgr = Ime::ComPtr<ITfUIElementMgr>::queryFrom(threadMgr());
            if(elementMgr) {
                BOOL pbShow = false;
                if(validComposingWindowElementId = (elementMgr->BeginUIElement(composingWindow, &pbShow, &composingWindowElementId) == S_OK)) {
                    composingWindow->Show(pbShow);
                }
            }
        }
    }

    void TextService::updateComposingWindow(Ime::EditSession *session, std::wstring *composing) {
        createComposingWindow(session);
        if(!composingWindow) return;

        composingWindow->clear();
        composingWindow->add(*composing, ' ');
        composingWindow->recalculateSize();
        composingWindow->refresh();

        RECT textRect;
        if(selectionRect(session, &textRect)) {
            composingWindow->move(textRect.left, textRect.bottom);
        }

        if(validComposingWindowElementId) {
            auto elementMgr = Ime::ComPtr<ITfUIElementMgr>::queryFrom(threadMgr());
            if(elementMgr) {
                elementMgr->UpdateUIElement(composingWindowElementId);
            }
        }
    }

    void TextService::hideComposingWindow() {
        if(validComposingWindowElementId) {
            auto elementMgr = Ime::ComPtr<ITfUIElementMgr>::queryFrom(threadMgr());
            if(elementMgr) {
                elementMgr->EndUIElement(composingWindowElementId);
                composingWindowElementId = 0;
                validComposingWindowElementId = false;
            }
        }
        if(composingWindow) {
            composingWindow = nullptr;
        }
    }

    void TextService::createCandidateWindow(Ime::EditSession *session) {
        if(!candidateWindow) {
            candidateWindow = new Ime::CandidateWindow(this, session);
            candidateWindow->Release();

            candidateWindow->setFont(candidateFont);

            auto elementMgr = Ime::ComPtr<ITfUIElementMgr>::queryFrom(threadMgr());
            if(elementMgr) {
                BOOL pbShow = false;
                if(validCandidateWindowElementId = (elementMgr->BeginUIElement(candidateWindow, &pbShow, &candidateWindowElementId) == S_OK)) {
                    candidateWindow->Show(pbShow);
                }
            }
        }
    }

    void TextService::updateCandidateWindow(Ime::EditSession *session, std::vector<std::wstring> *candidates) {
        createCandidateWindow(session);
        if(!candidateWindow) return;

        candidateWindow->clear();
        int index = 0;
        for(auto &candidate : *candidates) {
            char ch = '1' + index;
            candidateWindow->add(candidate, ch);
            index += 1;
        }

        candidateWindow->recalculateSize();
        candidateWindow->refresh();

        RECT textRect, composingWindowRect;
        if(selectionRect(session, &textRect)) {
            int height = 0;
            if(composingWindow) {
                composingWindow->rect(&composingWindowRect);
                height = composingWindowRect.bottom - composingWindowRect.top;
            }
            candidateWindow->move(textRect.left, textRect.bottom + height * 2);
        }

        if(validCandidateWindowElementId) {
            auto elementMgr = Ime::ComPtr<ITfUIElementMgr>::queryFrom(threadMgr());
            if(elementMgr) {
                elementMgr->UpdateUIElement(candidateWindowElementId);
            }
        }
    }

    void TextService::hideCandidateWindow() {
        if(validCandidateWindowElementId) {
            auto elementMgr = Ime::ComPtr<ITfUIElementMgr>::queryFrom(threadMgr());
            if(elementMgr) {
                elementMgr->EndUIElement(candidateWindowElementId);
                candidateWindowElementId = 0;
                validCandidateWindowElementId = false;
            }
        }
        if(candidateWindow) {
            candidateWindow = nullptr;
        }
    }

    Ime::CandidateWindow *TextService::getCandidateWindow() {
        return this->candidateWindow;
    }

    int TextService::fontHeight(int size) {
        int fontHeight = size;
        HDC hdc = GetDC(NULL);
        if(hdc) {
            fontHeight = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
            ReleaseDC(NULL, hdc);
        }
        return fontHeight;
    }
}
