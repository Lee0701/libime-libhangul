#include "ManualConversionInputMode.h"

namespace HangulIME {
    ManualConversionInputMode::ManualConversionInputMode(char *keyboardType) : InputMode() {
        this->keyboardType = keyboardType;
        hic = hangul_ic_new(keyboardType);
        this->composing = L"";
    }

    ManualConversionInputMode::~ManualConversionInputMode() {
        hangul_ic_delete(hic);
    }

    void ManualConversionInputMode::onActivate() {
        hangul_ic_reset(hic);
    }

    void ManualConversionInputMode::onDeactivate() {
        hangul_ic_reset(hic);
    }

    bool ManualConversionInputMode::testEditKey(int code) {
        switch(code) {
        case VK_BACK:
        case VK_SPACE:
        case VK_RETURN:
            return !hangul_ic_is_empty(hic);
        }
        return false;
    }

    bool ManualConversionInputMode::onEditKey(void *context, int code) {
        InputContext *inputContext = (InputContext *) context;
        if(code == VK_BACK) {
            bool result = hangul_ic_backspace(hic);
            this->compose(inputContext, &preedit_str());
            return result;
        }
        this->flush(inputContext);
        return false;
    }

    void ManualConversionInputMode::onChar(void *context, int code) {
        InputContext *inputContext = (InputContext *) context;
        bool res = hangul_ic_process(hic, code);
        this->commit(inputContext, &commit_str());
        this->compose(inputContext, &preedit_str());
        if(!res) {
            std::wstring commit = L"";
            commit += (wchar_t) code;
            this->commit(inputContext, &commit);
        }
    }

    void ManualConversionInputMode::onReset(void *context) {
        this->flush((InputContext *) context);
    }

    void ManualConversionInputMode::compose(InputContext *context, std::wstring *str) {
        this->composing = *str;
        context->compose(str);
    }

    void ManualConversionInputMode::commit(InputContext *context, std::wstring *str) {
        context->commit(str);
        this->composing = L"";
    }

    void ManualConversionInputMode::flush(InputContext *context) {
        context->commit(&this->composing);
        hangul_ic_reset(hic);
        this->composing = L"";
    }

    std::wstring ManualConversionInputMode::commit_str() {
        std::wstring commit = L"";
        const ucschar *str = hangul_ic_get_commit_string(hic);
        if(str[0] == 0) return commit;
        const size_t str_len = sizeof(str) / sizeof(ucschar);
        for(int i = 0 ; i < str_len / 2 ; i++) {
            commit += (wchar_t) str[i * 2];
        }
        return commit;
    }

    std::wstring ManualConversionInputMode::preedit_str() {
        std::wstring preedit = L"";
        const ucschar *str = hangul_ic_get_preedit_string(hic);
        if(str[0] == 0) return preedit;
        const size_t str_len = sizeof(str) / sizeof(ucschar);
        for(int i = 0 ; i < str_len / 2 ; i++) {
            preedit += (wchar_t) str[i * 2];
        }
        return preedit;
    }

};
