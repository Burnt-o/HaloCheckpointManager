#pragma once
#include "Hotkey.h"
#include "HotkeyDefinitions.h"
#include "IMessagesGUI.h"


class HotkeyManager
{
private:
	// callbacks
	ScopedCallback<RenderEvent> mImGuiRenderCallbackHandle;

	// services
	std::shared_ptr<HotkeyDefinitions> mHotkeyDefinitions;


	// data 
	std::string mHotkeyConfigPath;
	bool mDisableHotkeysForRebinding = false;

	void pollInput();


public:
	// Hotkey manager doesn't render, but it does want to poll for inputs every frame
	HotkeyManager(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr<HotkeyDefinitions> pHotkeyDefinitions, std::shared_ptr<IMessagesGUI> messagesGUI, std::string dirPath);
	~HotkeyManager();

	void setDisableHotkeysForRebinding(bool val) { mDisableHotkeysForRebinding = val; };

};

