#include "HangulIMESettings.h"
#include <fstream>
#include "ini.h"

namespace HangulIME {
    void HangulIMESettings::defaultSettings() {
        hangulKeyboardType = "2";
        hanjaConversionMode = "auto";
        candidateFontSize = 14;
    }

    void HangulIMESettings::loadSettings() {
        fs::path settingsPath = userDir / "settings.ini";
        if (fs::exists(settingsPath)) {
            inih::INIReader r(settingsPath.string());
            hangulKeyboardType = r.Get<std::string>("ime", "hangul_keyboard_type");
            hanjaConversionMode = r.Get<std::string>("ime", "hanja_conversion_mode");
            candidateFontSize = r.Get<int>("ime", "candidate_font_size");
        } else {
            defaultSettings();
            saveSettings();
        }
    }

    void HangulIMESettings::saveSettings() {
        if (!fs::exists(userDir)) {
            fs::create_directories(userDir);
        }
        fs::path settingsPath = userDir / "settings.ini";

        inih::INIReader r;
        r.InsertEntry("ime", "hangul_keyboard_type", hangulKeyboardType);
        r.InsertEntry("ime", "hanja_conversion_mode", hanjaConversionMode);
        r.InsertEntry("ime", "candidate_font_size", std::to_string(candidateFontSize));

        inih::INIWriter::write(settingsPath.string(), r);
    }
}
