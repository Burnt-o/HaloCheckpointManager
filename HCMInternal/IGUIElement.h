#pragma once
#include "pch.h"
#include "GameState.h"
#include "imgui.h"
#include "HotkeyRenderer.h"
class IGUIElement
{
protected:
	std::optional<HotkeysEnum> mHotkey;
	float currentHeight = 0;
	GameState mImplGame;
	bool requiredServicesReady = true;


public:
	float getCurrentHeight() { return this->currentHeight; }
	virtual void render(HotkeyRenderer&) = 0;

	IGUIElement(GameState implGame, std::optional<HotkeysEnum> hotkey)
		: mImplGame(implGame), mHotkey(hotkey)
	{
	
	}

	virtual std::string_view getName() = 0;

	virtual ~IGUIElement() = default;
};

template <typename T>
concept GUIElementBaseTemplate = std::is_base_of<IGUIElement, T>::value;


