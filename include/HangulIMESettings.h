#pragma once
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace HangulIME {
    class HangulIMESettings {
    private:
        fs::path installDir;
        fs::path userDir;

    public:
        std::string hangulKeyboardType;
        std::string hanjaConversionMode;
        int candidateFontSize;

        HangulIMESettings(fs::path installDir, fs::path userDir) : installDir(installDir), userDir(userDir) {
        }

        ~HangulIMESettings() {
        }

        void loadSettings();
        void saveSettings();

        fs::path getInstallDir() const {
            return this->installDir;
        }

        fs::path getUserDir() const {
            return this->userDir;
        }
    };
}
