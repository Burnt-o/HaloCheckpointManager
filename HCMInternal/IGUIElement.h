#pragma once
#include "pch.h"
#include "GameState.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "HotkeyRenderer.h"
#include "ToolTipCollection.h"
class IGUIElement
{
protected:
	std::optional<HotkeysEnum> mHotkey;
	float currentHeight = 0;
	GameState mImplGame;
	bool requiredServicesReady = true;
	ToolTipCollection toolTipCollection;


public:
	float getCurrentHeight() { return this->currentHeight; }
	virtual void render(HotkeyRenderer&) = 0;

	IGUIElement(GameState implGame, std::optional<HotkeysEnum> hotkey, ToolTipCollection tooltip)
		: mImplGame(implGame), mHotkey(hotkey), toolTipCollection(tooltip)
	{
	
	}

	virtual std::string_view getName() = 0;

	virtual ~IGUIElement() = default;

	std::optional<HotkeysEnum> getHotkey() { return mHotkey; }

	void renderTooltip()
	{
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip(toolTipCollection.getToolTip(mImplGame).data());
		}
	}
};

template <typename T>
concept GUIElementBaseTemplate = std::is_base_of<IGUIElement, T>::value;


