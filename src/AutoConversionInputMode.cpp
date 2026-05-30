#include "AutoConversionInputMode.h"
#include "strcvt.h"

namespace HangulIME {
    AutoConversionInputMode::AutoConversionInputMode(std::filesystem::path installDir, char *keyboardType) : InputMode() {
        this->hic = hangul_ic_new(keyboardType);
        std::filesystem::path hanjaTablePath = installDir / "hanja" / "hanja.txt";
        this->htbl = hanja_table_load(hanjaTablePath.string().c_str());
        std::vector<std::string> tablePaths;
        tablePaths.push_back((installDir / "hanja" / "freq-hanja.txt").string());
        tablePaths.push_back((installDir / "hanja" / "freq-hanjaeo.txt").string());
        this->hfreq = new HanjaFrequencyTable(tablePaths);

        this->composing = L"";
        this->converted = L"";
        this->locked = L"";
        this->candidates = new CandidateState(9);
    }

    AutoConversionInputMode::~AutoConversionInputMode() {
        hangul_ic_delete(hic);
        hanja_table_delete(htbl);
        delete this->hfreq;
        delete this->candidates;
    }

    void AutoConversionInputMode::onActivate() {
    }

    void AutoConversionInputMode::onDeactivate() {
    }

    bool AutoConversionInputMode::testEditKey(int code) {
        switch(code) {
        case VK_BACK:
        case VK_SPACE:
        case VK_RETURN:
        case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
            return composing.length() > 0;
        }
        return true;
    }

    bool AutoConversionInputMode::onEditKey(void *context, int code) {
        InputContext *ic = (InputContext *) context;
        if(inputs.size() == 0) {
            this->flush(ic);
            this->update(ic);
            return false;
        }
        switch(code) {
        case VK_BACK:
            inputs.pop_back();
            break;
        case VK_SPACE:
            if(locked.length() == 0 && candidates->getIndex() == 0) {
                inputs.push_back(' ');
                candidates->setIndex(0);
            } else {
                commitCurrent(ic);
            }
            break;
        case VK_RETURN:
            this->flush(ic);
            break;
        case VK_LEFT:
            break;
        case VK_RIGHT:
            break;
        case VK_UP:
            candidates->moveCursor(-1);
            break;
        case VK_DOWN:
            candidates->moveCursor(1);
            break;
        default:
            this->flush(ic);
            this->update(ic);
            return false;
        }
        this->update(ic);
        return true;
    }

    bool AutoConversionInputMode::onChar(void *context, int code) {
        inputs.push_back(code);
        candidates->setIndex(0);
        this->update((InputContext *) context);
        return true;
    }

    void AutoConversionInputMode::onReset(void *context) {
        this->inputs.clear();
        this->candidates->clearCandidates();
        this->composing = L"";
        this->converted = L"";
        this->locked = L"";
        InputContext *ic = (InputContext *) context;
        ic->updateComposingWindow(&converted);
        ic->updateCandidateWindow(&candidates->getPageCandidates());
    }

    void AutoConversionInputMode::update(InputContext *context) {
        int index = candidates->getIndex();
        updateComposing();
        updateCandidates();
        updateConverted();
        context->updateComposingWindow(&converted);
        context->updateCandidateWindow(&candidates->getPageCandidates());
        context->setSelectedIndex(candidates->getPageIndex());
    }

    void AutoConversionInputMode::updateCandidates() {
        if(this->htbl == nullptr) return;

        this->candidates->clearCandidates();
        const int lockedLen = (int) this->locked.length();
        const int searchLen = min(10, (int) this->composing.length() - lockedLen);
        int maxCandLen = min(1, (int) this->composing.length() - lockedLen);
        for(int i = (int) searchLen ; i >= 1 ; i--) {
            std::string str = ws2s(this->composing.substr(lockedLen, i));
            HanjaList *list = hanja_table_match_exact(htbl, str.c_str());

            int len = hanja_list_get_size(list);
            for(int k = 0 ; k < len ; k++) {
                std::wstring cand = s2ws(hanja_list_get_nth_value(list, k));
                this->candidates->addCandidate(cand.substr(0, i));
            }
            if(len > 0 && i > maxCandLen) {
                maxCandLen = i;
            }

            hanja_list_delete(list);
        }

        this->candidates->sortCandidates(hfreq);

        if(maxCandLen > 0) {
            this->candidates->insertCandidate(0, this->composing.substr(lockedLen, maxCandLen));
        }
    }

    void AutoConversionInputMode::updateComposing() {
        this->composing = combineHangul();
        if(composing.length() > 0 && this->locked.length() > composing.length() - 1) {
            this->locked = this->locked.substr(0, composing.length() - 1);
        }
    }

    void AutoConversionInputMode::updateConverted() {
        std::wstring selected = this->candidates->getCandidate();
        converted = locked + selected + composing.substr(locked.length() + selected.length());
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
        this->onReset(context);
    }

    void AutoConversionInputMode::flush(InputContext *context) {
        this->commit(context, &this->converted);
    }

    void AutoConversionInputMode::commitCurrent(InputContext *context) {
        std::wstring selected = this->candidates->getCandidate();
        this->locked += selected;
        if(converted.length() > locked.length() && converted[locked.length()] == L' ') {
            this->locked += L' ';
        }
        candidates->setIndex(0);
        if(locked.length() >= converted.length()) {
            this->flush(context);
        }
    }
}
