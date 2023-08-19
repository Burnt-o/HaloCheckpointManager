#pragma once
#include "HaloEnums.h"
#include "CheatBase.h"
#include "ForceCheckpoint.h"
#include "ForceRevert.h"
#include "ForceDoubleRevert.h"
#include "CheckpointInjectDump.h"

class CheatManager
{
private:

	template <CheatBaseTemplate T>
	static void constructUninitializedCheat(std::map<GameState, std::shared_ptr<CheatBase>>& collection)
	{
		for (auto game : AllSupportedGames)
		{
			std::shared_ptr<T> cheat = std::make_shared<T>(game);
			collection.emplace(game, cheat);
		}
	}

public:

	static std::map<GameState, std::shared_ptr<CheatBase>> forceCheckpointCollection;
	static std::map<GameState, std::shared_ptr<CheatBase>> forceRevertCollection;
	static std::map<GameState, std::shared_ptr<CheatBase>> forceDoubleRevertCollection;
	static std::map<GameState, std::shared_ptr<CheatBase>> checkpointInjectDumpCollection;


	static void ConstructAllCheatsUninitialized()
	{
		constructUninitializedCheat<ForceCheckpoint>(forceCheckpointCollection);
		constructUninitializedCheat<ForceRevert>(forceRevertCollection);
		constructUninitializedCheat<ForceDoubleRevert>(forceDoubleRevertCollection);
		constructUninitializedCheat<CheckpointInjectDump>(checkpointInjectDumpCollection);
	}



};

