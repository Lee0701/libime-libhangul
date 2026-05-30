#include "HangulIMETextService.h"
#include <filesystem>
#include <ShlObj_core.h>
#include "InputContext.h"
#include "AsciiInputMode.h"
#include "ManualConversionInputMode.h"
#include "AutoConversionInputMode.h"

namespace HangulIME {
    TextService::TextService(ImeModule *module) : Ime::TextService(module) {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(module->hInstance(), path, sizeof(path));
        std::filesystem::path installDir = std::filesystem::path(path).parent_path();
        SHGetFolderPathW(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, 0, path);
        std::filesystem::path userDir = std::filesystem::path(path) / "libime-libhangul";
        if (!std::filesystem::exists(userDir)) {
            std::filesystem::create_directories(userDir);
        }
        this->settings = new HangulIMESettings(installDir, userDir);
    }

    TextService::~TextService() {
        delete this->settings;
    }

    void TextService::onActivate() {
        this->settings->loadSettings();
        this->settings->saveSettings();

        const char *hangulKeyboardType = settings->hangulKeyboardType.c_str();
        if(settings->hanjaConversionMode == "manual") {
            this->hangulInputMode = new ManualConversionInputMode(settings);
        } else if(settings->hanjaConversionMode == "auto") {
            this->hangulInputMode = new AutoConversionInputMode(settings);
        } else {
            this->hangulInputMode = new InputMode();
        }
        this->asciiInputMode = new AsciiInputMode();
        this->currentInputMode = asciiInputMode;

        LOGFONT lf;
        composingFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
        GetObject(composingFont, sizeof(lf), &lf);
        lf.lfHeight = fontHeight(settings->candidateFontSize);
        lf.lfWeight = FW_NORMAL;
        composingFont = CreateFontIndirect(&lf);

        candidateFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
        GetObject(candidateFont, sizeof(lf), &lf);
        lf.lfHeight = fontHeight(settings->candidateFontSize);
        lf.lfWeight = FW_NORMAL;
        candidateFont = CreateFontIndirect(&lf);

        currentInputMode->onActivate();
    }

    void TextService::onDeactivate() {
        currentInputMode->onDeactivate();

        hideComposingWindow();
        hideCandidateWindow();

        delete this->asciiInputMode;
        delete this->hangulInputMode;
    }

    void TextService::onFocus() {
    }

    bool TextService::filterKeyDown(Ime::KeyEvent& keyEvent) {
        int keyCode = keyEvent.keyCode();
        bool controlPressed = keyEvent.isKeyDown(VK_CONTROL) || keyEvent.isKeyDown(VK_LCONTROL) || keyEvent.isKeyDown(VK_RCONTROL);
        // For Type 3 keyboard driver
        if(controlPressed && keyCode != VK_HANGUL && keyCode != VK_HANJA) {
            return false;
        }
        bool result = keyEvent.isChar();
        switch(keyCode) {
        case VK_BACK:
        case VK_SPACE:
        case VK_RETURN:
        case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
            result = currentInputMode->testEditKey(keyCode);
            break;
        case VK_ESCAPE:
            result = true;
            break;
        case VK_HANGUL:
            result = true;
            break;
        case VK_HANJA:
            result = currentInputMode->testEditKey(keyCode);
        }
        if(!result) {
            hideComposingWindow();
            hideCandidateWindow();
        }
        return result;
    }

    bool TextService::onKeyDown(Ime::KeyEvent& keyEvent, Ime::EditSession *session) {
        InputContext context(session, this);
        int keyCode = keyEvent.keyCode();
        bool controlPressed = keyEvent.isKeyDown(VK_CONTROL) || keyEvent.isKeyDown(VK_LCONTROL) || keyEvent.isKeyDown(VK_RCONTROL);
        if(controlPressed && keyCode != VK_HANGUL && keyCode != VK_HANJA) {
            currentInputMode->onReset(&context);
            return false;
        }
        switch(keyCode) {
        case VK_ESCAPE:
            currentInputMode->onReset(&context);
            return true;
        case VK_HANGUL:
            currentInputMode->onReset(&context);
            currentInputMode->onDeactivate();
            if(currentInputMode == hangulInputMode) {
                currentInputMode = asciiInputMode;
            } else {
                currentInputMode = hangulInputMode;
            }
            currentInputMode->onActivate();
            return true;
        case VK_HANJA:
            return currentInputMode->onEditKey(&context, keyCode);
        case VK_BACK:
        case VK_SPACE:
        case VK_RETURN:
        case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
            return currentInputMode->onEditKey(&context, keyCode);
        }
        return currentInputMode->onChar(&context, keyEvent.charCode());
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
            candidateWindow->move(textRect.left, textRect.bottom + height / 2 * 3);
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
