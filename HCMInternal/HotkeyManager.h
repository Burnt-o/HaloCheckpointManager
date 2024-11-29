#pragma once
#include "HotkeyDefinitions.h"
#include "IMessagesGUI.h"
#include "SharedRequestProvider.h"

class HotkeyManager
{
private:
	// callbacks
	ScopedCallback<RenderEvent> mImGuiRenderCallbackHandle;

	// services
	std::shared_ptr<HotkeyDefinitions> mHotkeyDefinitions;
	std::shared_ptr<TokenSharedRequestProvider> mHotkeyDisabler;


	// data 
	std::string mHotkeyConfigPath;
	std::vector<std::thread> mFireEventThreads;

	void pollInput();


public:
	// Hotkey manager doesn't render, but it does want to poll for inputs every frame
	HotkeyManager(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr<HotkeyDefinitions> pHotkeyDefinitions, std::shared_ptr<IMessagesGUI> messagesGUI, std::string dirPath, std::shared_ptr<TokenSharedRequestProvider> hotkeyDisabler);
	~HotkeyManager();

	void saveHotkeysToFile();

};

