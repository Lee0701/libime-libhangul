#pragma once
#include <string>
#include <vector>

namespace HangulIME {
    class CandidateState {
    private:
        std::vector<std::wstring> candidates;
        int pageSize;
        int index;

    public:
        CandidateState(int pageSize);
        virtual ~CandidateState();

        void moveCursor(int direction);
        void movePage(int direction);
        void setPage(int page);
        int getPage() const;
        int getIndex() const;
        void setIndex(int index);
        int getPageIndex() const;
        int getPageCount() const;

        std::wstring getCandidate() const;
        void clearCandidates();
        void addCandidate(const std::wstring &candidate);
        void insertCandidate(int index, const std::wstring &candidate);
        void removeCandidate(int index);
        std::vector<std::wstring> getPageCandidates() const;
    };
}
