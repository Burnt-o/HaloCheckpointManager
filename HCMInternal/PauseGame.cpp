#include "pch.h"
#include "PauseGame.h"
#include "ModuleHook.h"
#include "MidhookContextInterpreter.h"

// to avoid ambiguity of updateService, these intermediate classes rename those functions
class IPauseService : public TokenSharedRequestProvider
{ 
public: 
	~IPauseService() = default; 
	virtual void updateService() override { updatePauseService(); }
	virtual void updatePauseService() = 0;
};
class IOverridePauseService : public TokenSharedRequestProvider
{
public:
	~IOverridePauseService() = default;
	virtual void updateService() override { updateOverrideService(); }
	virtual void updateOverrideService() = 0;
};


class PauseGame::PauseGameImpl : public IPauseService, public IOverridePauseService
{
private:
	std::vector<std::unique_ptr<ModulePatch>> pauseGamePatches;

public:
	std::map<GameState, HCMInitException> serviceFailures;

	PauseGameImpl(std::shared_ptr<PointerDataStore> ptr)
	{
		for (auto game : AllSupportedGames)
		{
			try
			{
				auto pauseGameFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(pauseGameFunction), game);
				auto pauseGameCode = ptr->getVectorData<byte>(nameof(pauseGameCode), game);

				std::unique_ptr<ModulePatch> patch = ModulePatch::make(game.toModuleName(), pauseGameFunction, *pauseGameCode.get(), false);

				pauseGamePatches.emplace_back(std::move(patch));
			}
			catch (HCMInitException ex)
			{
				serviceFailures.emplace(game, ex);
			}
		}

		if (pauseGamePatches.empty())
		{
			std::stringstream oss;
			for (auto& [game, ex] : serviceFailures)
			{
				oss << game.toString() << ": " << ex.what() << std::endl;
			}
			throw HCMInitException(std::format("Could not construct any game implementations\n{}", oss.str()));
		}

	}

	~PauseGameImpl()
	{
		PLOG_DEBUG << "~" << nameof(PauseGameImpl);
	}
	

	std::shared_ptr<SharedRequestToken> makeRegularScopedRequest() { return IPauseService::makeScopedRequest(); }
	std::shared_ptr<SharedRequestToken> makeOverrideScopedRequest() { return IOverridePauseService::makeScopedRequest(); }

	void combinedUpdateService()
	{

		bool requested = this->IPauseService::serviceIsRequested() && !this->IOverridePauseService::serviceIsRequested();

		PLOG_INFO << "PauseGame service is turning " << (requested ? "ON!" : "OFF!");
		PLOG_DEBUG << "this->IPauseService::serviceIsRequested(): " << (this->IPauseService::serviceIsRequested() ? "true" : "false");
		PLOG_DEBUG << "this->IOverridePauseService::serviceIsRequested(): " << (this->IOverridePauseService::serviceIsRequested() ? "true" : "false");

		for (auto& patch : pauseGamePatches)
		{
			patch->setWantsToBeAttached(requested);
		}

		PLOG_DEBUG << "PauseGame hooks have been updated.";

	}

	virtual void updatePauseService() override { combinedUpdateService(); }
	virtual void updateOverrideService() override { combinedUpdateService(); }
};







PauseGame::PauseGame(std::shared_ptr<PointerDataStore> ptr)
	: pimpl(std::make_shared< PauseGameImpl>(ptr))
{}

PauseGame::~PauseGame() = default;

std::shared_ptr<SharedRequestToken> PauseGame::makeScopedRequest() { return pimpl->makeRegularScopedRequest(); }
std::shared_ptr<SharedRequestToken> PauseGame::makeOverrideScopedRequest() { return pimpl->makeOverrideScopedRequest(); }

std::map<GameState, HCMInitException>& PauseGame::getServiceFailures() { return pimpl->serviceFailures; }