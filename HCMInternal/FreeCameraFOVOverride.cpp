#include "pch.h"
#include "FreeCameraFOVOverride.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"

template <GameState::Value gameT>
class FreeCameraFOVOverrideImplVoid : public TokenSharedRequestProvider
{
public:
	// does nothing lol
	FreeCameraFOVOverrideImplVoid(GameState game, IDIContainer& dicon) {}
};

template <GameState::Value gameT>
class FreeCameraFOVOverrideImplModulePatch : public TokenSharedRequestProvider
{
private:

	std::shared_ptr<ModulePatch> freeCameraFOVOverridePatch;

	virtual void updateService() override 
	{
		PLOG_DEBUG << "FreeCameraFOVOverrideImplModulePatch setting to state:" << (serviceIsRequested() ? "true" : "false");
		freeCameraFOVOverridePatch->setWantsToBeAttached(serviceIsRequested());
	}

public:
	FreeCameraFOVOverrideImplModulePatch(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto freeCameraFOVOverrideFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(freeCameraFOVOverrideFunction), game);
		auto freeCameraFOVOverrideCode = ptr->getVectorData<byte>(nameof(freeCameraFOVOverrideCode), game);
		freeCameraFOVOverridePatch = ModulePatch::make(game.toModuleName(), freeCameraFOVOverrideFunction, *freeCameraFOVOverrideCode.get());
	}
};

FreeCameraFOVOverride::FreeCameraFOVOverride(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_shared<FreeCameraFOVOverrideImplVoid<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_shared<FreeCameraFOVOverrideImplModulePatch<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_shared<FreeCameraFOVOverrideImplModulePatch<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_shared<FreeCameraFOVOverrideImplModulePatch<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_shared<FreeCameraFOVOverrideImplVoid<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_shared<FreeCameraFOVOverrideImplVoid<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

FreeCameraFOVOverride::~FreeCameraFOVOverride() = default;