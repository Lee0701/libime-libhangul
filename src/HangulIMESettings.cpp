#include "HangulIMESettings.h"
#include <fstream>
#include "ini.h"

namespace HangulIME {
    void HangulIMESettings::loadSettings() {
        fs::path settingsPath = userDir / "settings.ini";
        inih::INIReader r;
        if(fs::exists(settingsPath)) r = inih::INIReader(settingsPath.string());
        hangulKeyboardType = r.Get<std::string>("ime", "hangul_keyboard_type", "2");
        hanjaConversionMode = r.Get<std::string>("ime", "hanja_conversion_mode", "auto");
        candidateFontSize = r.Get<int>("ime", "candidate_font_size", 14);
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

        inih::INIWriter::write(settingsPath.string(), r, true);
    }
}
