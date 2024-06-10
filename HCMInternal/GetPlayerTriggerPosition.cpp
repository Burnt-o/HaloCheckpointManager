#include "pch.h"
#include "GetPlayerTriggerPosition.h"
#include "DynamicStructFactory.h"
#include "PointerDataStore.h"
#include "GetPlayerDatum.h"
#include "GetObjectAddress.h"
#include "IMakeOrGetCheat.h"

template <GameState::Value gameT>
class GetPlayerTriggerPositionImpl : public GetPlayerTriggerPosition::IGetPlayerTriggerPosition
{
private:

	// injected services
	std::weak_ptr< GetPlayerDatum> getPlayerDatumWeak;
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;

	// data
	enum class bipedDataFields { triggerHitVertex };
	std::shared_ptr<DynamicStruct<bipedDataFields>> playerDataStruct;



public:
	GetPlayerTriggerPositionImpl(GameState game, IDIContainer& dicon)
		:
		getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum)),
		getObjectAddressWeak(resolveDependentCheat(GetObjectAddress))

	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		playerDataStruct = DynamicStructFactory::make<bipedDataFields>(ptr, game);
	}

	virtual const SimpleMath::Vector3 getPlayerTriggerPosition() override
	{
		lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		
		playerDataStruct->currentBaseAddress = getObjectAddress->getObjectAddress(getPlayerDatum->getPlayerDatum());

		auto* result = playerDataStruct->field<SimpleMath::Vector3>(bipedDataFields::triggerHitVertex);
		if (IsBadReadPtr(result, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad triggerHitVertex read at {}", (uintptr_t)result));

		return *result;
		 
	}
};













GetPlayerTriggerPosition::GetPlayerTriggerPosition(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<GetPlayerTriggerPositionImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<GetPlayerTriggerPositionImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<GetPlayerTriggerPositionImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<GetPlayerTriggerPositionImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<GetPlayerTriggerPositionImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<GetPlayerTriggerPositionImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}


}