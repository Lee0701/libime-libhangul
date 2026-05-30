#include "ManualConversionInputMode.h"

namespace HangulIME {
    ManualConversionInputMode::ManualConversionInputMode(std::filesystem::path installDir, char *keyboardType) : InputMode() {
        hic = hangul_ic_new(keyboardType);
        this->converter = new HanjaConverter(installDir / "hanja");
        this->composing = L"";
        this->candidates = new CandidateState(9);
    }

    ManualConversionInputMode::~ManualConversionInputMode() {
        hangul_ic_delete(hic);
        delete this->converter;
        delete this->candidates;
    }

    void ManualConversionInputMode::onActivate() {
        hangul_ic_reset(hic);
        composing = L"";
    }

    void ManualConversionInputMode::onDeactivate() {
        hangul_ic_reset(hic);
        composing = L"";
    }

    bool ManualConversionInputMode::testEditKey(int code) {
        switch(code) {
        case VK_BACK:
        case VK_SPACE:
        case VK_RETURN:
            return !hangul_ic_is_empty(hic);
        case VK_HANJA:
            return this->composing.length() > 0;
        case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
            return this->candidates->hasCandidates();
        }
        return false;
    }

    bool ManualConversionInputMode::onEditKey(void *context, int code) {
        InputContext *ic = (InputContext *) context;
        switch(code) {
        case VK_BACK:
            // Clear the candidates first
            this->candidates->clearCandidates();
            ic->updateCandidateWindow(nullptr);
            if(!hangul_ic_is_empty(hic)) {
                bool result = hangul_ic_backspace(hic);
                this->compose(ic, &preedit_str());
                return result;
            }
        case VK_HANJA:
            if(this->composing.length() > 0) {
                this->candidates->clearCandidates();
                this->candidates->setIndex(0);
                std::vector<std::wstring> candidates = converter->convert(this->composing);
                if(candidates.size() == 0) return true;
                for(auto &candidate : candidates) {
                    this->candidates->addCandidate(candidate);
                }
                ic->updateCandidateWindow(&this->candidates->getPageCandidates());
                return true;
            }
        case VK_UP:
            if(this->candidates->hasCandidates()) {
                candidates->moveCursor(-1);
                ic->updateCandidateWindow(&this->candidates->getPageCandidates());
                ic->setSelectedIndex(candidates->getPageIndex());
                return true;
            }
        case VK_DOWN:
            if(this->candidates->hasCandidates()) {
                candidates->moveCursor(1);
                ic->updateCandidateWindow(&this->candidates->getPageCandidates());
                ic->setSelectedIndex(candidates->getPageIndex());
                return true;
            }
        case VK_RETURN:
        case VK_SPACE:
            if(this->candidates->hasCandidates()) {
                this->commit(ic, &this->candidates->getCandidate());
                this->flush(ic);
                return true;
            }
        }
        this->flush(ic);
        return false;
    }

    bool ManualConversionInputMode::onChar(void *context, int code) {
        InputContext *ic = (InputContext *) context;

        // Hide candidates first before processing hangul
        this->candidates->clearCandidates();
        ic->updateCandidateWindow(nullptr);

        bool res = hangul_ic_process(hic, code);
        this->commit(ic, &commit_str());
        this->compose(ic, &preedit_str());
        if(!res) {
            std::wstring commit(1,(wchar_t) code);
            this->commit(ic, &commit);
        }
        return true;
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
        this->candidates->clearCandidates();
        context->updateCandidateWindow(nullptr);
        context->commit(&this->composing);
        hangul_ic_reset(hic);
        this->composing = L"";
    }

    std::wstring ManualConversionInputMode::commit_str() {
        std::wstring commit = L"";
        const ucschar *str = hangul_ic_get_commit_string(hic);
        if(str[0] == 0) return commit;
        const size_t str_len = sizeof(str) / sizeof(ucschar);
        for(int i = 0 ; i < str_len ; i++) {
            if(!str[i]) continue;
            commit += (wchar_t) str[i];
        }
        return commit;
    }

    std::wstring ManualConversionInputMode::preedit_str() {
        std::wstring preedit = L"";
        const ucschar *str = hangul_ic_get_preedit_string(hic);
        if(str[0] == 0) return preedit;
        const size_t str_len = sizeof(str) / sizeof(ucschar);
        for(int i = 0 ; i < str_len ; i++) {
            if(!str[i]) continue;
            preedit += (wchar_t) str[i];
        }
        return preedit;
    }
}
