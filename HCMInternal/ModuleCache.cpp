#include "pch.h"
#include "ModuleCache.h"


// Adapted from scales' MCCTAS https://github.com/Scaless/HaloTAS/blob/master/HaloTAS/libhalotas/dll_cache.cpp
std::unordered_map<std::wstring, MODULEINFO> ModuleCache::mCache = {};

void ModuleCache::initialize()
{

	// Clear the cache
	ModuleCache::mCache.clear();

	// Fill with current values
	HMODULE hMods[1024];
	HANDLE hProcess = GetCurrentProcess();
	DWORD cbNeeded;

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			if (GetModuleBaseName(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
				MODULEINFO info;
				if (GetModuleInformation(hProcess, hMods[i], &info, sizeof(info))) {
					std::wstring name{ szModName };
					ModuleCache::mCache.emplace(name, info);
				}
			}
		}
	}
}

bool ModuleCache::addModuleToCache(const std::wstring& moduleName, HMODULE moduleHandle)
{

	// check if it's in the cache already
	if (auto it = ModuleCache::mCache.find(moduleName); it != ModuleCache::mCache.end()) {
		return true;
	}

	// if not, add to cache
	MODULEINFO info;
	if (GetModuleInformation(GetCurrentProcess(), moduleHandle, &info, sizeof(info))) {
		ModuleCache::mCache.emplace(moduleName, info);
		return true;
	}
	else 
	{
		return false;
	}


}

bool ModuleCache::removeModuleFromCache(const std::wstring& moduleName)
{
	if (auto it = ModuleCache::mCache.find(moduleName); it != ModuleCache::mCache.end()) {
		ModuleCache::mCache.erase(it);
		return true;
	}

	return false;
}

std::optional<HMODULE> ModuleCache::getModuleHandle(const std::wstring& moduleName)
{
	if (auto it = ModuleCache::mCache.find(moduleName); it != ModuleCache::mCache.end()) {
		return (HMODULE)it->second.lpBaseOfDll;
	}

	return std::nullopt;
}

std::optional<MODULEINFO> ModuleCache::getModuleInfo(const std::wstring& moduleName)
{
	if (auto it = ModuleCache::mCache.find(moduleName); it != ModuleCache::mCache.end()) {
		return it->second;
	}

	return std::nullopt;
}

bool ModuleCache::isModuleInCache(const std::wstring& moduleName)
{
	auto it = ModuleCache::mCache.find(moduleName);
	return it != ModuleCache::mCache.end();

}