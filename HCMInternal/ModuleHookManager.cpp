#include "pch.h"
#include "ModuleHookManager.h"
#include "WindowsUtilities.h"

#include "ModuleCache.h"




ModuleHookManager::ModuleHookManager()
{

	// We'll make global hooks here that track dll loading/unloading
	PLOG_INFO << "Hooking module load/unload";

	mHook_LoadLibraryA = safetyhook::create_inline(&LoadLibraryA, &newLoadLibraryA);
	mHook_LoadLibraryW = safetyhook::create_inline(&LoadLibraryW, &newLoadLibraryW);
	mHook_LoadLibraryExA = safetyhook::create_inline(&LoadLibraryExA, &newLoadLibraryExA);
	mHook_LoadLibraryExW = safetyhook::create_inline(&LoadLibraryExW, &newLoadLibraryExW); // Crashes on safetyhook 0.5.3 due to shared mem page with VirtualProtect.
	mHook_FreeLibrary = safetyhook::create_inline(&FreeLibrary, &newFreeLibrary);

	//mModuleHooksMap.reserve(6); // probably only the 6 game dll's that we might care about ever
	PLOG_VERBOSE << "Hooking module load/unload done";
}

ModuleHookManager::~ModuleHookManager()
{
	PLOG_VERBOSE << "ModuleHookManager destructor called";

	// We must detach the module-relative hooks before unhooking the library load/unload functions
	// otherwise we could have a stale reference issue
	detachAllHooks();

	mHook_LoadLibraryA.reset();
	mHook_LoadLibraryW.reset();
	mHook_LoadLibraryExA.reset();
	mHook_LoadLibraryExW.reset();
	mHook_FreeLibrary.reset();

}


// This only called on global_kill
void ModuleHookManager::detachAllHooks() {
	// Get all hook elements and detach them
	for (auto& [moduleName, hookVector] : mModuleHooksMap)
	{
		for (auto& hook : hookVector)
		{
			hook->detach();
		}
	}

}





// Called by newFreeLibrary, unhooks any hooks tied to the unloading module
void ModuleHookManager::preModuleUnload_UpdateHooks(std::wstring_view libFilename) 
{
	// Get a ref to the module-hooks map
	const auto& moduleHooksMap = mModuleHooksMap;

	// Is the currently unloading library in our map?
	auto it = moduleHooksMap.find(libFilename.data());

	if (it == moduleHooksMap.end()) return; // Module isn't in our map, we're done here

	PLOG_DEBUG << "pre_lib_unload_hooks_patches detaching hooks for: " << libFilename;
	// Iterate thru the vector of hooks associated with this module and detach them all
	for (auto& hook : it->second)
	{
			hook->detach();
	}
}

// Called by newLoadLibraries, attaches any hooks (that want to be enabled) associated with the loading module
void ModuleHookManager::postModuleLoad_UpdateHooks(std::wstring_view libPath) 
{
	// Need to get the name of the module from the libPath arguement
	std::filesystem::path path = libPath;
	auto libFilename = path.filename().generic_wstring();

	// Get a ref to the module-hooks map
	const auto& moduleHooksMap = mModuleHooksMap;

	// Is the currently loading module in our map?
	auto it = moduleHooksMap.find(libFilename.data());

	if (it == moduleHooksMap.end()) return; // Module isn't in our map, we're done here

	PLOG_DEBUG << "post_lib_load_hooks_patches attaching hooks for: " << libFilename;
	// Iterate thru the vector of hooks associated with this module and attach all the wants that have their wantToBeAttached flag set
	for (auto& hook : it->second)
	{
		if (hook->getWantsToBeAttached())
		{
			hook->attach();
		}
		else // It wants to be disabled
		{
			hook->detach();
		}
	}
}



// For debugging/logging the below functions, logs only if the affected module is one of the ones we care about
#ifdef HCM_DEBUG

std::set<std::wstring> gameDLLNames{ L"halo1.dll", L"halo2.dll", L"halo3.dll", L"halo3odst.dll", L"haloreach.dll", L"halo4.dll", L"graphics-hook64.dll" };

#define printGameDLL(dllname) if (gameDLLNames.contains(dllname)) \
						{	\
							PLOG_INFO << wstr_to_str(dllname);	\
						}	\

#endif // HCM_DEBUG



std::wstring filenameFromFilepath(std::wstring in)
{
	if (in.contains('\\') == false) return in;

	std::wstring out = in.substr(in.find_last_of(L"/\\") + 1);
	return out;
}



// the hook-redirected functions
HMODULE ModuleHookManager::newLoadLibraryA(LPCSTR lpLibFileName) {
	auto result =mHook_LoadLibraryA.call< HMODULE, LPCSTR > (lpLibFileName);

	auto newLib = filenameFromFilepath(str_to_wstr(lpLibFileName));

#ifdef HCM_DEBUG
	printGameDLL(newLib)
#endif

	ModuleCache::addModuleToCache(newLib, result);
	postModuleLoad_UpdateHooks(newLib);

	return result;
}

HMODULE ModuleHookManager::newLoadLibraryW(LPCWSTR lpLibFileName) {
	auto result = mHook_LoadLibraryW.call<HMODULE, LPCWSTR>(lpLibFileName);

	auto newLib = filenameFromFilepath(lpLibFileName);

#ifdef HCM_DEBUG
	printGameDLL(newLib)
#endif

	ModuleCache::addModuleToCache(newLib, result);
	postModuleLoad_UpdateHooks(newLib);

	return result;
}

HMODULE ModuleHookManager::newLoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
	auto result = mHook_LoadLibraryExA.call<HMODULE, LPCSTR, HANDLE, DWORD>(lpLibFileName, hFile, dwFlags);

	auto newLib = filenameFromFilepath(str_to_wstr(lpLibFileName));



#ifdef HCM_DEBUG
	printGameDLL(newLib)
#endif
;
	ModuleCache::addModuleToCache(newLib, result);
	postModuleLoad_UpdateHooks(newLib);

	return result;
}

HMODULE ModuleHookManager::newLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
	auto result = mHook_LoadLibraryExW.call<HMODULE, LPCWSTR, HANDLE, DWORD>(lpLibFileName, hFile, dwFlags);

	auto newLib = filenameFromFilepath(lpLibFileName);

#ifdef HCM_DEBUG
	printGameDLL(newLib)
#endif
	ModuleCache::addModuleToCache(newLib, result);
	postModuleLoad_UpdateHooks(newLib);

	return result;
}

BOOL ModuleHookManager::newFreeLibrary(HMODULE hLibModule) {

	wchar_t moduleFilePath[MAX_PATH];
	GetModuleFileName(hLibModule, moduleFilePath, sizeof(moduleFilePath) / sizeof(TCHAR));



	std::filesystem::path path = moduleFilePath;
	auto filename = path.filename().generic_wstring();

#ifdef HCM_DEBUG
	PLOGV << "newFreeLibrary";
	printGameDLL(filename)
#endif

	preModuleUnload_UpdateHooks(filename);
	ModuleCache::removeModuleFromCache(filename);
	return mHook_FreeLibrary.call<BOOL, HMODULE>(hLibModule);
}

