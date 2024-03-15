#include "pch.h"
#include "PauseGame.h"
#include "ModuleHook.h"
#include "MidhookContextInterpreter.h"





class PauseGame::PauseGameImpl : public IProvideScopedRequests
{
private:
	static inline PauseGameImpl* instance = nullptr;

	std::set<std::string> callersRequestingBlockedInput{};
	std::vector<std::unique_ptr<ModulePatch>> pauseGamePatches;



	bool isOverriden = false;

public:
	std::map<GameState, HCMInitException> serviceFailures;

	PauseGameImpl(std::shared_ptr<PointerDataStore> ptr)
	{
		if (instance) throw HCMInitException("Cannot have more than one PauseGameImpl");

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


		instance = this;
	}

	~PauseGameImpl()
	{
		PLOG_DEBUG << "~" << nameof(PauseGameImpl);

		instance = nullptr;
	}
	void requestService(std::string callerID)
	{
		callersRequestingBlockedInput.insert(callerID);
		if (callersRequestingBlockedInput.empty() == false && !isOverriden)
		{
			for (auto& patch : pauseGamePatches)
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
			for (auto& patch : pauseGamePatches)
			{
				patch->setWantsToBeAttached(false);
			}
		}
	}


	void setOverride(bool overrideValue)
	{
		isOverriden = overrideValue;
		if (overrideValue)
		{
			for (auto& patch : pauseGamePatches)
			{
				patch->setWantsToBeAttached(false);
			}
		}
		else
		{
			if (callersRequestingBlockedInput.empty() == false)
			{
				for (auto& patch : pauseGamePatches)
				{
					patch->setWantsToBeAttached(true);
				}
			}
		}
	}

};

class PauseGame::OverridePauseGameImpl : public IProvideScopedRequests
{
private:

	std::set<std::string> callersRequestingBlockedInput{};
	std::weak_ptr<PauseGame::PauseGameImpl> pauseGameImpl;

public:
	void requestService(std::string callerID)
	{
		PLOG_DEBUG << "OverridePauseGameImpl requested";
		callersRequestingBlockedInput.insert(callerID);
		if (callersRequestingBlockedInput.empty() == false)
		{
			auto pauseLock = pauseGameImpl.lock();
			if (!pauseLock) { PLOG_ERROR << "bad pauseGameImpl weak ptr"; return; }
			pauseLock->setOverride(true);
		}
	}

	void unrequestService(std::string callerID)
	{
		PLOG_DEBUG << "OverridePauseGameImpl unrequested";
		callersRequestingBlockedInput.erase(callerID);
		if (callersRequestingBlockedInput.empty() == true)
		{
			auto pauseLock = pauseGameImpl.lock();
			if (!pauseLock) { PLOG_ERROR << "bad pauseGameImpl weak ptr"; return; }
			pauseLock->setOverride(false);
		}
	}

	OverridePauseGameImpl(std::weak_ptr<PauseGame::PauseGameImpl> pauseGameImp)
		: pauseGameImpl(pauseGameImp)
	{

	}

};








PauseGame::PauseGame(std::shared_ptr<PointerDataStore> ptr)
	: pimpl(std::make_shared< PauseGameImpl>(ptr)),
	 overridePimpl(std::make_shared< OverridePauseGameImpl>(pimpl))

{}

PauseGame::~PauseGame() = default;

std::unique_ptr<ScopedServiceRequest> PauseGame::scopedRequest(std::string callerID) { return std::make_unique<ScopedServiceRequest>(pimpl, callerID); }

std::unique_ptr<ScopedServiceRequest> PauseGame::scopedOverrideRequest(std::string callerID) { return std::make_unique<ScopedServiceRequest>(overridePimpl, callerID); }

std::map<GameState, HCMInitException>& PauseGame::getServiceFailures() { return pimpl->serviceFailures; }