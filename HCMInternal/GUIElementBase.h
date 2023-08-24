#pragma once
#include "pch.h"
#include "GameState.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"
#include "CheatBase.h"
#include "FailedServiceInfo.h"


class GUIElementBase
{
protected:
	float currentHeight = 0;
	GameState mImplGame;
	bool requiredServicesReady = true;

public:
	float getCurrentHeight() { return this->currentHeight; }
	virtual void render() = 0;

	GUIElementBase(GameState implGame, std::vector<std::map<GameState, std::shared_ptr<CheatBase>>> requiredServices)
		: mImplGame(implGame)
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


