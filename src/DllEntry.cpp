#include "HangulIMEModule.h"
#include <vector>
#include <VersionHelpers.h>

HangulIME::ImeModule *gImeModule = NULL;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		::DisableThreadLibraryCalls(hModule); // disable DllMain calls due to new thread creation
		gImeModule = new HangulIME::ImeModule(hModule);
		break;
	case DLL_PROCESS_DETACH:
		if(gImeModule) {
			gImeModule->Release();
			gImeModule = NULL;
		}
		break;
	}
	return TRUE;
}

STDAPI DllCanUnloadNow(void) {
	return gImeModule->canUnloadNow();
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObj) {
	return gImeModule->getClassObject(rclsid, riid, ppvObj);
}

STDAPI DllUnregisterServer(void) {
	return gImeModule->unregisterServer();
}

STDAPI DllRegisterServer(void) {
	int iconIndex = 0; // use classic icon
	if(::IsWindows8OrGreater()) {
		iconIndex = 1; // use Windows 8 style IME icon
    }
	std::vector<Ime::LangProfileInfo> langProfiles;
	auto name = L"HangulIME";
	GUID guid = {
        0x20ac5250,
        0x62a0,
        0x48a2,
        {0x9c, 0xd2, 0x7a, 0x30, 0x95, 0x32, 0x6f, 0x61}
    };
	auto locale = L"ko";
	auto fallbackLocale = L"en";
	Ime::LangProfileInfo langProfile = {
		name,
		guid,
		locale,
		fallbackLocale
	};
	langProfiles.push_back(std::move(langProfile));
	return gImeModule->registerServer(L"HangulIME", langProfiles.data(), langProfiles.size());
}
