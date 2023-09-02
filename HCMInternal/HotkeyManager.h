#pragma once
#include "Hotkey.h"



// pimpl
class HotkeyManager
{
private:
	class HotkeyManagerImpl;
	std::unique_ptr< HotkeyManagerImpl> pimpl;
	static inline HotkeyManager* instance = nullptr;

	std::string mHotkeyConfigPath;

public:
	// Hotkey manager doesn't render, but it does want to poll for inputs every frame
	HotkeyManager(eventpp::CallbackList<void()>& pRenderEvent);
	~HotkeyManager();


};

