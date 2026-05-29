#include "HangulImeModule.h"
#include "HangulIMETextService.h"

namespace HangulIME {

    const CLSID gHangulIME_CLSID = {
        0xdfca125f,
        0x837f,
        0x4dc4,
        {0xa5, 0x0f, 0x8c, 0xbf, 0x0d, 0x6b, 0x0f, 0x39}
    };

    ImeModule::ImeModule(HMODULE module) : Ime::ImeModule(module, gHangulIME_CLSID) {
    }

    ImeModule::~ImeModule(void) {
    }

    // virtual
    Ime::TextService* ImeModule::createTextService() {
        Ime::TextService *service = new HangulIME::TextService(this);
        return service;
    }

}
