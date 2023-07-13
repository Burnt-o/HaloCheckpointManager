#pragma once
#include "HaloEnums.h"
#include "CheatBase.h"
#include "ForceCheckpoint.h"



class CheatManager
{
private:
	static CheatManager* instance;

	template <CheatBaseTemplate T>
	void constructCheatCollection(std::map<GameState, std::shared_ptr<T>>& collection)
	{

		// try actually constructing
		for (auto game : AllSupportedGames)
		{
			std::shared_ptr<T> cheat = std::make_shared<T>(game);
			collection.emplace(game, cheat);

			if (!cheat.get()->getSupportedGames().contains(game)) continue;

			try
			{
				cheat.get()->initialize();
			}
			catch(HCMInitException ex)
			{
				ex.prepend(std::format("Could not create cheat service: {}::{}, Service will be unavailable.\n", GameStateToString.at(game), "ah"));
				RuntimeExceptionHandler::handleMessage(ex);
			}
		}
	}

public:

	std::map<GameState, std::shared_ptr<ForceCheckpoint>> forceCheckpointCollection;


	CheatManager()
	{
		if (instance) throw HCMInitException("Cannot have more than one CheatManager");
		instance = this;

		// Construct cheats
		constructCheatCollection(forceCheckpointCollection);

	}
	~CheatManager()
	{
		instance = nullptr;
	}


};

