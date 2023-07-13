#pragma once
#include "ModuleHook.h"



// Singleton that works with ModuleCache to manage ModuleHooks,
// attaching and detaching hooks as necessary when modules load/unload)
class ModuleHookManager
{
private:
	friend class ModuleInlineHook;
	friend class ModuleMidHook;
	friend class ModulePatch;
	static ModuleHookManager* instance;


	// Where we keep all the module relative hooks (key = name of module, value = list of all hooks for that module).
	std::unordered_map	<std::wstring, std::vector
		<
		ModuleHookBase*
		>
	> mModuleHooksMap;


	// Hooks on module load/unload functions.
	// These are attached on ModuleHookManager construction
	// These are automagically cleaned up (detached) on ModuleHookManager destruction
	safetyhook::InlineHook mHook_LoadLibraryA;
	safetyhook::InlineHook mHook_LoadLibraryW;
	safetyhook::InlineHook mHook_LoadLibraryExA;
	safetyhook::InlineHook mHook_LoadLibraryExW;
	safetyhook::InlineHook mHook_FreeLibrary;

	// So we can process the hooks we need to attach/detach of the loading/unloading library
	static HMODULE newLoadLibraryA(LPCSTR lpLibFileName);
	static HMODULE newLoadLibraryW(LPCWSTR lpLibFileName);
	static HMODULE newLoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
	static HMODULE newLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
	static BOOL newFreeLibrary(HMODULE hLibModule);


	static void preModuleUnload_UpdateHooks(std::wstring_view libFilename); // called by newFreeLibrary
	static void postModuleLoad_UpdateHooks(std::wstring_view libPath); // called by newLoadLibraries

	void detachAllHooks();

	static void addHook(const std::wstring& moduleName, ModuleHookBase* newHook)
	{
		// Note: [] operator creates the moduleName key if it didn't already exist in the map
		instance->mModuleHooksMap[moduleName].emplace_back(newHook);
	}


	static void removeHook(const std::wstring& moduleName, ModuleHookBase* hookToRemove)
	{
		if (instance->mModuleHooksMap.contains(moduleName))
		{
			auto position = std::ranges::find(instance->mModuleHooksMap[moduleName], hookToRemove);

			if (position == instance->mModuleHooksMap[moduleName].end())
				return;

			instance->mModuleHooksMap[moduleName].erase(position);
		}
	}

public:

	ModuleHookManager();

	~ModuleHookManager();





};


