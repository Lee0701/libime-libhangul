#pragma once
#include <ImeModule.h>

namespace HangulIME {
    class ImeModule : public Ime::ImeModule {
    public:
        ImeModule(HMODULE module);
        virtual ~ImeModule(void);

        virtual Ime::TextService* createTextService();
    };
}
