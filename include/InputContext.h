#pragma once
#include <string>
#include <EditSession.h>
#include "HangulIMETextService.h"

namespace HangulIME {
    class InputContext {
    private:
        Ime::EditSession *session;
        TextService *service;

    public:
        InputContext(Ime::EditSession *session, TextService *service) : session(session), service(service) {}

        void compose(std::wstring *text);
        void commit(std::wstring *text);

        void updateComposingWindow(std::wstring *composing);
        void updateCandidateWindow(std::vector<std::wstring> *candidates);

        int getSelectedIndex();
        void setSelectedIndex(int index);
    };
}
