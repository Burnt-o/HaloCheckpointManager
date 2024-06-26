#include "pch.h"
#include "GetScenarioAddress.h"
#include "MultilevelPointer.h"
#include "IMCCStateHook.h"
#include "PointerDataStore.h"

template <GameState::Value gameT>
class GetScenarioAddressPointerImpl : public IGetScenarioAddressImpl
{
private:
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> mccStateChangedCallback;

	std::shared_ptr<MultilevelPointer> scenarioAddress;
	uintptr_t scenarioAddressCached;

	bool cacheValid = false;

public:
	GetScenarioAddressPointerImpl(GameState gameImpl, IDIContainer& dicon)
		: mccStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState&) { cacheValid = false; }),
		scenarioAddress(dicon.Resolve<PointerDataStore>().lock()->getData<std::shared_ptr<MultilevelPointer>>(nameof(scenarioAddress), gameImpl))
	{
		if (scenarioAddress->resolve(&scenarioAddressCached))
			cacheValid = true;
	}

	virtual std::expected<uintptr_t, HCMRuntimeException> getScenarioAddress() override
	{
		if (!cacheValid)
		{
			if (!scenarioAddress->resolve(&scenarioAddressCached))
				return std::unexpected(HCMRuntimeException(std::format("Could not resolve scenarioAddress, error: {}", MultilevelPointer::GetLastError())));

			cacheValid = true;
		}

		return scenarioAddressCached;

	}
};

GetScenarioAddress::GetScenarioAddress(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<GetScenarioAddressPointerImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<GetScenarioAddressPointerImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<GetScenarioAddressPointerImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<GetScenarioAddressPointerImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<GetScenarioAddressPointerImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<GetScenarioAddressPointerImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;

	default:
		throw HCMInitException("Not impl yet");
	}
}
GetScenarioAddress::~GetScenarioAddress() = default;
std::expected<uintptr_t, HCMRuntimeException> GetScenarioAddress::getScenarioAddress() { return pimpl->getScenarioAddress(); }