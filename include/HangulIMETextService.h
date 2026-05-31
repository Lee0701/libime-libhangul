#pragma once
#include <TextService.h>
#include <MessageWindow.h>
#include <CandidateWindow.h>
#include "HangulIMEModule.h"
#include "HangulIMESettings.h"
#include "InputMode.h"

namespace HangulIME {
    class TextService : public Ime::TextService {
    private:
        HangulIMESettings *settings;

        BOOL validComposingWindowElementId;
        DWORD composingWindowElementId;
        Ime::ComPtr<Ime::CandidateWindow> composingWindow;
        HFONT composingFont;

        BOOL validCandidateWindowElementId;
        DWORD candidateWindowElementId;
        Ime::ComPtr<Ime::CandidateWindow> candidateWindow;
        HFONT candidateFont;

        InputMode *hangulInputMode;
        InputMode *asciiInputMode;
        InputMode *currentInputMode;

    public:
        TextService(ImeModule *module);

        virtual void onActivate();
        virtual void onDeactivate();

        virtual void onFocus();

        virtual bool filterKeyDown(Ime::KeyEvent& keyEvent);
        virtual bool onKeyDown(Ime::KeyEvent& keyEvent, ITfContext* context);

        virtual bool filterKeyUp(Ime::KeyEvent& keyEvent);
        virtual bool onKeyUp(Ime::KeyEvent& keyEvent, ITfContext* context);

        virtual bool onPreservedKey(const GUID& guid);

        virtual bool onCommand(UINT id, CommandType type);

        // called when a compartment value is changed
        virtual void onCompartmentChanged(const GUID& key);

        // called when the keyboard is opened or closed
        virtual void onKeyboardStatusChanged(bool opened);

        // called just before current composition is terminated for doing cleanup.
        // if forced is true, the composition is terminated by others, such as
        // the input focus is grabbed by another application.
        // if forced is false, the composition is terminated gracefully by endComposition().
        virtual void onCompositionTerminated(bool forced);

        void createComposingWindow(ITfContext* context);
        void updateComposingWindow(ITfContext* context, std::wstring *composing);
        void hideComposingWindow();

        void createCandidateWindow(ITfContext* context);
        void updateCandidateWindow(ITfContext* context, std::vector<std::wstring> *candidates);
        void hideCandidateWindow();

        Ime::CandidateWindow *getCandidateWindow();

    private:
        virtual ~TextService(void);

        int fontHeight(int size);
    };

}
