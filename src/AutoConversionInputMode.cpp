#include "AutoConversionInputMode.h"
#include "strcvt.h"

namespace HangulIME {
    AutoConversionInputMode::AutoConversionInputMode(std::filesystem::path installDir, char *keyboardType) : InputMode() {
        this->hic = hangul_ic_new(keyboardType);
        std::filesystem::path hanjaTablePath = installDir / "hanja" / "hanja.txt";
        this->htbl = hanja_table_load(hanjaTablePath.string().c_str());
        this->composing = L"";
    }

    AutoConversionInputMode::~AutoConversionInputMode() {
        hangul_ic_delete(hic);
        hanja_table_delete(htbl);
    }

    void AutoConversionInputMode::onActivate() {
    }

    void AutoConversionInputMode::onDeactivate() {
    }

    bool AutoConversionInputMode::testEditKey(int code) {
        return true;
    }

    bool AutoConversionInputMode::onEditKey(void *context, int code) {
        InputContext *ic = (InputContext *) context;
        switch(code) {
        case VK_BACK:
            if(inputs.size() > 0) {
                inputs.pop_back();
                this->update(ic);
                return true;
            }
            break;
        case VK_SPACE:
        case VK_RETURN:
            break;
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
            break;
        }
        this->flush(ic);
        this->update(ic);
        return false;
    }

    void AutoConversionInputMode::onChar(void *context, int code) {
        inputs.push_back(code);
        this->update((InputContext *) context);
    }

    void AutoConversionInputMode::onReset(void *context) {
    }

    void AutoConversionInputMode::update(InputContext *context) {
        updateComposing();
        updateCandidates();

        context->updateComposingWindow(&composing);
        context->updateCandidateWindow(&candidates);
    }

    void AutoConversionInputMode::updateCandidates() {
        if(this->htbl == nullptr) return;

        this->candidates.clear();
        const int searchLen = min(10, (int) this->composing.length());
        int maxCandLen = 0;
        for(int i = (int) searchLen ; i >= 1 ; i--) {
            std::string str = ws2s(this->composing.substr(0, i));
            HanjaList *list = hanja_table_match_exact(htbl, str.c_str());

            int len = hanja_list_get_size(list);
            for(int k = 0 ; k < len ; k++) {
                std::wstring cand = s2ws(hanja_list_get_nth_value(list, k));
                this->candidates.push_back(cand);
                if(cand.length() > maxCandLen) {
                    maxCandLen = (int) cand.length();
                }
            }

            hanja_list_delete(list);
        }
        if(maxCandLen > 0) {
            this->candidates.push_back(this->composing.substr(0, maxCandLen));
        }
    }

    void AutoConversionInputMode::updateComposing() {
        this->composing = combineHangul();
    }

    std::wstring AutoConversionInputMode::combineHangul() {
        hangul_ic_reset(hic);
        std::wstring result = L"";
        for(int &code : this->inputs) {
            bool res = hangul_ic_process(hic, code);
            if(!res) {
                const ucschar *commit = hangul_ic_get_commit_string(hic);
                if(commit[0] != 0) {
                    result += (wchar_t) commit[0];
                }
                hangul_ic_flush(hic);
                result += (wchar_t) code;
            }
            const ucschar *commit = hangul_ic_get_commit_string(hic);
            if(commit[0] != 0) {
                result += (wchar_t) commit[0];
            }
        }
        const ucschar *preedit = hangul_ic_get_preedit_string(hic);
        if(preedit[0] != 0) {
            result += (wchar_t) preedit[0];
        }
        hangul_ic_reset(hic);
        return result;
    }

    void AutoConversionInputMode::compose(InputContext *context, std::wstring *str) {
        this->composing = *str;
        context->compose(str);
    }

    void AutoConversionInputMode::commit(InputContext *context, std::wstring *str) {
        context->commit(str);
        this->composing = L"";
    }

    void AutoConversionInputMode::flush(InputContext *context) {
        context->commit(&this->composing);
        this->inputs.clear();
        this->composing = L"";
    }

}
