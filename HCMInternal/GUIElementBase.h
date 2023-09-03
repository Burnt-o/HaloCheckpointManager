#pragma once
#include "pch.h"
#include "GameState.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"
#include "CheatBase.h"
#include "FailedServiceInfo.h"
#include "Hotkey.h"
#include "HotkeyRebinder.h"
class GUIElementBase
{
private:
	std::optional<std::shared_ptr<Hotkey>> mHotkey;

protected:
	float currentHeight = 0;
	GameState mImplGame;
	bool requiredServicesReady = true;
	virtual std::string_view getHotkeyButtonLabelId() = 0; // = std::format("...#{}", getName());

	// renders a hotkey if we have it, or blank padding space if we don't. If you don't want either than just don't call renderHotkey.
	void renderHotkey()
	{
		if (!mHotkey)
		{
			ImGui::Dummy({ 93,20 }); // padding
		}
		else
		{
			ImGui::PushID(getName().data());
			ImGui::BeginChild("HotkeyChild", { 63, 20 });
			ImGui::AlignTextToFramePadding();
			ImGui::Text(mHotkey.value().get()->getBindingTextShort().data());
			addTooltip(std::format("{} hotkeys: {}", getName(), mHotkey.value().get()->getBindingText()).c_str());
			ImGui::EndChild();

			ImGui::SameLine();

			// button for rebinding hotkey
			if (ImGui::Button("..")) // todo: may need to adjust button size
			{
				ImGui::PopID(); // pop id before calling modal dialog
				HotkeyRebinder::beginHotkeyRebindDialog(mHotkey.value());
			}
			else
			{
				ImGui::PopID(); // id must always be popped once either way
			}
			addTooltip(std::format("Rebind hotkeys for {}", getName()).c_str());


		}
	}
public:
	float getCurrentHeight() { return this->currentHeight; }
	virtual void render() = 0;

	GUIElementBase(GameState implGame, std::vector<std::map<GameState, std::shared_ptr<CheatBase>>> requiredServices, std::optional<std::shared_ptr<Hotkey>> hotkey)
		: mImplGame(implGame), mHotkey(hotkey)
	{
		std::vector<std::string> namesOfFailedRequiredServices;
		for (auto cheatService : requiredServices)
		{
			auto* cheat = cheatService.at(implGame).get();
			if (cheat->attemptedInitialization == false)
			{
				cheat->initializeBase();
			}

			if (!cheat->successfullyInitialized)
			{
				namesOfFailedRequiredServices.emplace_back(implGame.toString() + "::" + std::string{cheat->getName()});
			}

			requiredServicesReady = requiredServicesReady && cheat->successfullyInitialized;
		}

		if (!requiredServicesReady)
		{
			FailedServiceInfo::addGUIFalure(implGame, std::bind(&GUIElementBase::getName, this), namesOfFailedRequiredServices);
		}

	}

	bool areRequiredServicesReady() { return requiredServicesReady; }
	virtual std::string_view getName() = 0;
};

template <typename T>
concept GUIElementBaseTemplate = std::is_base_of<GUIElementBase, T>::value;


