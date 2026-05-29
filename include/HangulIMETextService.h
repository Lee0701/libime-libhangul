#pragma once
#include <TextService.h>
#include <MessageWindow.h>
#include <CandidateWindow.h>
#include "HangulIMEModule.h"
#include "InputMode.h"

namespace HangulIME {
    class TextService : public Ime::TextService {
    private:
        BOOL validComposingWindowElementId;
        DWORD composingWindowElementId;
        Ime::ComPtr<Ime::CandidateWindow> composingWindow;

        BOOL validCandidateWindowElementId;
        DWORD candidateWindowElementId;
        Ime::ComPtr<Ime::CandidateWindow> candidateWindow;

        InputMode *inputMode;

    public:
        TextService(ImeModule *module);

        virtual void onActivate();
        virtual void onDeactivate();

        virtual void onFocus();

        virtual bool filterKeyDown(Ime::KeyEvent& keyEvent);
        virtual bool onKeyDown(Ime::KeyEvent& keyEvent, Ime::EditSession *session);

        virtual bool filterKeyUp(Ime::KeyEvent& keyEvent);
        virtual bool onKeyUp(Ime::KeyEvent& keyEvent, Ime::EditSession *session);

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

        void createComposingWindow(Ime::EditSession *session);
        void updateComposingWindow(Ime::EditSession *session);
        void hideComposingWindow();

        void createCandidateWindow(Ime::EditSession *session);
        void updateCandidateWindow(Ime::EditSession *session);
        void hideCandidateWindow();

        void compose(Ime::EditSession *session, std::wstring *text);
        void commit(Ime::EditSession *session, std::wstring *text);

    private:
        virtual ~TextService(void);
    };

}
