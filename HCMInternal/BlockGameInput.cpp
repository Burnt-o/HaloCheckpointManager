#include "pch.h"
#include "BlockGameInput.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"




class BlockGameInput::BlockGameInputImpl : public IProvideScopedRequests
{
private:
	static inline BlockGameInputImpl* instance = nullptr;

	std::set<std::string> callersRequestingBlockedInput{};
	std::map<GameState, std::shared_ptr<ModulePatch>> blockGameInputPatches;

	std::map<GameState, HCMInitException> serviceFailures;
public:
	BlockGameInputImpl(std::shared_ptr<PointerManager> ptr)
	{
		if (instance) throw HCMInitException("Cannot have more than one BlockGameInputImpl");


		for (auto game : AllSupportedGames)
		{
			try
			{
				auto blockGameInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(blockGameInputFunction), game);
				auto blockGameInputCode = ptr->getVectorData<byte>(nameof(blockGameInputCode), game);

				std::shared_ptr<ModulePatch> patch;
				switch (game) // runtime gamevalue to template gamevalue shennaigans
				{
				case GameState::Value::Halo1: patch = ModulePatch::make(game.toModuleName(), blockGameInputFunction, *blockGameInputCode.get(), false); break;
				case GameState::Value::Halo2: patch = ModulePatch::make(game.toModuleName(), blockGameInputFunction, *blockGameInputCode.get(), false); break;
				case GameState::Value::Halo3: patch = ModulePatch::make(game.toModuleName(), blockGameInputFunction, *blockGameInputCode.get(), false); break;
				case GameState::Value::Halo3ODST: patch = ModulePatch::make(game.toModuleName(), blockGameInputFunction, *blockGameInputCode.get(), false); break;
				case GameState::Value::HaloReach: patch = ModulePatch::make(game.toModuleName(), blockGameInputFunction, *blockGameInputCode.get(), false); break;
				case GameState::Value::Halo4: patch = ModulePatch::make(game.toModuleName(), blockGameInputFunction, *blockGameInputCode.get(), false); break;
				}
				blockGameInputPatches.emplace(game, patch);
			}
			catch (HCMInitException ex)
			{
				serviceFailures.emplace(game, ex);
			}
		}

		if (blockGameInputPatches.empty())
		{
			std::stringstream oss;
			for (auto& [game, ex] : serviceFailures)
			{
				oss << game.toString() << ": " << ex.what() << std::endl;
			}
			throw HCMInitException(std::format("Could not construct any game implementations\n{}", oss.str()));
		}


		instance = this;
	}

	~BlockGameInputImpl()
	{
		PLOG_DEBUG << "~" << nameof(BlockGameInputImpl);

		instance = nullptr;
	}
	void requestService(std::string callerID)
	{
		callersRequestingBlockedInput.insert(callerID);
		if (callersRequestingBlockedInput.empty() == false)
		{
			for (auto& [game, patch] : blockGameInputPatches)
			{
				patch->setWantsToBeAttached(true);
			}
		}
	}

	void unrequestService(std::string callerID)
	{
		callersRequestingBlockedInput.erase(callerID);
		if (callersRequestingBlockedInput.empty() == true)
		{
			for (auto& [game, patch] : blockGameInputPatches)
			{
				patch->setWantsToBeAttached(false);
			}
		}
	}


	std::map<GameState, HCMInitException>& getServiceFailures() { return serviceFailures; }

};








BlockGameInput::BlockGameInput(std::shared_ptr<PointerManager> ptr)
	: pimpl(std::make_shared< BlockGameInputImpl>(ptr)) {}

BlockGameInput::~BlockGameInput() = default;

std::unique_ptr<ScopedServiceRequest> BlockGameInput::scopedRequest(std::string callerID) { return std::make_unique<ScopedServiceRequest>(pimpl, callerID); }


std::map<GameState, HCMInitException>& BlockGameInput::getServiceFailures() { return pimpl->getServiceFailures(); }