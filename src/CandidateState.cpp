#include "CandidateState.h"

namespace HangulIME {
    CandidateState::CandidateState(int pageSize) {
        this->pageSize = pageSize;
        index = 0;
    }

    CandidateState::~CandidateState() {
    }


    void CandidateState::moveCursor(int direction) {
        if (direction == -1) {
            index--;
        } else if (direction == 1) {
            index++;
        }
        if(index < 0) {
            index = candidates.size()-1;
        }
        if(index >= candidates.size()) {
            index = 0;
        }
    }

    void CandidateState::movePage(int direction) {
        int page = getPage();
        if(direction == -1) {
            page--;
        } else if(direction == 1) {
            page++;
        }
        if(page < 0) {
            page = getPageCount() - 1;
        }
        if(page >= getPageCount()) {
            page = 0;
        }
        setPage(page);
    }

    void CandidateState::setPage(int page) {
        this->index = (page * pageSize) + (index % pageSize);
    }

    int CandidateState::getPage() const {
        return index / pageSize;
    }

    int CandidateState::getIndex() const {
        return this->index;
    }

    void CandidateState::setIndex(int index) {
        this->index = index;
    }

    int CandidateState::getPageIndex() const {
        return this->index % pageSize;
    }

    int CandidateState::getPageCount() const {
        int rem = (int) candidates.size() % pageSize;
        return ((int) candidates.size() / pageSize) + (rem > 0 ? 1 : 0);
    }

    std::wstring CandidateState::getCandidate() const {
        if(index < 0 || index >= candidates.size()) return L"";
        return candidates[index];
    }

    void CandidateState::clearCandidates() {
        candidates.clear();
    }

    void CandidateState::addCandidate(const std::wstring &candidate) {
        candidates.push_back(candidate);
    }

    void CandidateState::insertCandidate(int index, const std::wstring &candidate) {
        candidates.insert(candidates.begin() + index, candidate);
    }

    void CandidateState::removeCandidate(int index) {
        candidates.erase(candidates.begin() + index);
    }

    std::vector<std::wstring> CandidateState::getPageCandidates() const {
        std::vector<std::wstring> result;
        int start = getPage() * pageSize;
        int end = std::min(start + pageSize, (int) candidates.size());
        for(int i = start ; i < end ; i++) {
            result.push_back(candidates[i]);
        }
        return result;
    }
}
