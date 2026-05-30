#include "AutoConversionInputMode.h"
#include "strcvt.h"

namespace HangulIME {
    AutoConversionInputMode::AutoConversionInputMode(HangulIMESettings *settings) : InputMode() {
        this->hic = hangul_ic_new(settings->hangulKeyboardType.c_str());
        hangul_ic_set_option(hic, HANGUL_IC_OPTION_AUTO_REORDER, settings->autoReorder);
        this->converter = new HanjaConverter(settings->getInstallDir() / "hanja");
        this->composing = L"";
        this->converted = L"";
        this->locked = L"";
        this->candidates = new CandidateState(9);
    }

    AutoConversionInputMode::~AutoConversionInputMode() {
        hangul_ic_delete(hic);
        delete this->converter;
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
            if(locked.length() == 0 && candidates->getIndex() == 0 && inputs[inputs.size() - 1] != ' ') {
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
        this->candidates->clearCandidates();
        const int lockedLen = (int) this->locked.length();
        const int searchLen = min(10, (int) this->composing.length() - lockedLen);
        int maxCandLen = min(1, (int) this->composing.length() - lockedLen);
        for(int i = (int) searchLen ; i >= 1 ; i--) {
            std::vector<std::wstring> candidates = this->converter->convert(composing.substr(lockedLen, i));
            for(auto &candidate : candidates) {
                this->candidates->addCandidate(candidate);
            }
            if(candidates.size() > 0 && i > maxCandLen) {
                maxCandLen = i;
            }
        }
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
            const ucschar *commit = hangul_ic_get_commit_string(hic);
            if(commit[0] != 0) {
                const size_t commit_len = sizeof(commit) / sizeof(ucschar);
                for(int i = 0 ; i < commit_len ; i++) {
                    if(!commit[i]) continue;
                    result += (wchar_t) commit[i];
                }
            }
            if(res == false) {
                result += (wchar_t) code;
            }
        }
        const ucschar *preedit = hangul_ic_get_preedit_string(hic);
        if(preedit[0] != 0) {
            const size_t preedit_len = sizeof(preedit) / sizeof(ucschar);
            for(int i = 0 ; i < preedit_len ; i++) {
                if(!preedit[i]) continue;
                result += (wchar_t) preedit[i];
            }
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
