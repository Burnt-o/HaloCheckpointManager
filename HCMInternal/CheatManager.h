#pragma once
#include "GameState.h"
#include "CheatBase.h"
#include "ForceCheckpoint.h"
#include "ForceRevert.h"
#include "ForceDoubleRevert.h"

#include "InjectCheckpoint.h"
#include "InjectCore.h"
#include "DumpCheckpoint.h"
#include "DumpCore.h"
#include "ForceCoreSave.h"
#include "ForceCoreLoad.h"
#include "Speedhack.h"
#include "Invulnerability.h"

class CheatManager
{
private:

	template <CheatBaseTemplate T>
	static void constructUninitializedCheat(std::map<GameState, std::shared_ptr<CheatBase>>& collection)
	{
		for (auto& game : AllSupportedGames)
		{
			std::shared_ptr<T> cheat = std::make_shared<T>(game);
			collection.emplace(game, cheat);
		}
	}

public:

	static inline std::map<GameState, std::shared_ptr<CheatBase>> forceCheckpointCollection{};
	static inline std::map<GameState, std::shared_ptr<CheatBase>> forceRevertCollection{};
	static inline std::map<GameState, std::shared_ptr<CheatBase>> forceDoubleRevertCollection{};

	static inline std::map<GameState, std::shared_ptr<CheatBase>> forceCoreSaveCollection{};
	static inline std::map<GameState, std::shared_ptr<CheatBase>> forceCoreLoadCollection{};

	static inline std::map<GameState, std::shared_ptr<CheatBase>> injectCheckpointCollection{};
	static inline std::map<GameState, std::shared_ptr<CheatBase>> dumpCheckpointCollection{};
	static inline std::map<GameState, std::shared_ptr<CheatBase>> injectCoreCollection{};
	static inline std::map<GameState, std::shared_ptr<CheatBase>> dumpCoreCollection{};

	static inline std::map<GameState, std::shared_ptr<CheatBase>> speedhackCollection{};
	static inline std::map<GameState, std::shared_ptr<CheatBase>> invulnCollection{};



	static void ConstructAllCheatsUninitialized()
	{
		constructUninitializedCheat<ForceCheckpoint>(forceCheckpointCollection);
		constructUninitializedCheat<ForceRevert>(forceRevertCollection);
		constructUninitializedCheat<ForceDoubleRevert>(forceDoubleRevertCollection);

		constructUninitializedCheat<ForceCoreSave>(forceCoreSaveCollection);
		constructUninitializedCheat<ForceCoreLoad>(forceCoreLoadCollection);

		constructUninitializedCheat<InjectCheckpoint>(injectCheckpointCollection);
		constructUninitializedCheat<DumpCheckpoint>(dumpCheckpointCollection);
		constructUninitializedCheat<InjectCore>(injectCoreCollection);
		constructUninitializedCheat<DumpCore>(dumpCoreCollection);

		constructUninitializedCheat<Speedhack>(speedhackCollection);

		constructUninitializedCheat<Invulnerability>(invulnCollection);
	}



};

