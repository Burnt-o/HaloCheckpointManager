#pragma once
#include "pch.h"
#include "HaloEnums.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"
#include "CheatBase.h"

class GUIElementBase
{
protected:
	float currentHeight = 0;
	GameState mImplGame;
	bool requiredServicesReady = true;
public:
	float getCurrentHeight() { return this->currentHeight; }
	virtual void render() = 0;

	GUIElementBase(GameState implGame, std::map<GameState, std::shared_ptr<CheatBase>>& cheatService)
		: mImplGame(implGame)
	{
		auto* cheat = cheatService.at(implGame).get();
		if (cheat->attemptedInitialization == false)
		{
			cheat->initializeBase();
		}

		requiredServicesReady = requiredServicesReady && cheat->successfullyInitialized;
	}

	bool areRequiredServicesReady() { return requiredServicesReady; }
};

template <typename T>
concept GUIElementBaseTemplate = std::is_base_of<GUIElementBase, T>::value;


