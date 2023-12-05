#pragma once
#include "pch.h"
#include "GameState.h"
#include "GetAggroData.h"
#include "GetNextObjectDatum.h"
#include "GetCurrentRNG.h"

template <GameState::Value gameT>
class GetGameDataAsString
{
private:
	std::string dataStringA = "I have a big butt and my butt smells and I like to kiss my own butt";
	std::string dataStringB = "";

public:
	std::string_view getDataString(bool useDataStringA) { return useDataStringA ? dataStringA : dataStringB; }

	// setup stringstream formatting flags and preallocate some memory
	std::ostringstream ss = (static_cast<std::ostringstream&&>(std::ostringstream{} << std::setprecision(6) << std::fixed << std::showpos << std::string(200, '\0')));
	void updateGameData(bool useDataStringA, int gameTick)
	{
		ss.str("");


		if (showGameTick)
		{
			ss << std::noshowpos << std::dec << "Gametick: " << gameTick << std::showpos << std::hex << std::endl;
		}

		if (getAggroDataOptionalWeak.has_value())
		{
			lockOrThrow(getAggroDataOptionalWeak.value(), getAggroData);
			const auto currentAggroData = getAggroData->getAggroData();


			constexpr float gameTickRate = gameT == GameState::Value::Halo1 ? 30.f : 60.f;

			float decayTimerInSeconds = currentAggroData.aggroDecayTimer == 0 ? 0.f : ((float)currentAggroData.aggroDecayTimer / gameTickRate);

			ss << "Aggro Level: " << currentAggroData.aggroLevel << std::endl;
			ss << "Aggro Decay Timer: " << std::format("{:.2f}", decayTimerInSeconds) << "s" << std::endl;
			ss << "Player Has Aggro: " << (currentAggroData.playerHasAggro ? "True" : "False") << std::endl;
				 
		}

		if (getCurrentRNGOptionalWeak.has_value())
		{
			lockOrThrow(getCurrentRNGOptionalWeak.value(), getCurrentRNG);
			const auto currentRNG = getCurrentRNG->getCurrentRNG();

			ss << "RNG Seed: " << currentRNG << std::endl;

		}

		if (getNextObjectDatumOptionalWeak.has_value())
		{
			lockOrThrow(getNextObjectDatumOptionalWeak.value(), getNextObjectDatum);
			ss << "Next Object Datum: " << getNextObjectDatum->getNextObjectDatum() << std::endl;

		}




		if (useDataStringA)
		{
			dataStringA = ss.str();
		}
		else
		{
			dataStringB = ss.str();
		}
	}

	// optional injected services

	std::optional<std::weak_ptr<GetAggroData>> getAggroDataOptionalWeak;
	std::optional<std::weak_ptr<GetNextObjectDatum>> getNextObjectDatumOptionalWeak;
	std::optional<std::weak_ptr<GetCurrentRNG>> getCurrentRNGOptionalWeak = std::nullopt;
	bool showGameTick = false;
};



