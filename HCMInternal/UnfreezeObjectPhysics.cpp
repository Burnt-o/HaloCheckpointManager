#include "pch.h"
#include "UnfreezeObjectPhysics.h"
#include "ObjectTypes.h"
#include "GetObjectAddress.h"
#include "IMakeOrGetCheat.h"
#include "DynamicStructFactory.h"

class UnfreezeObjectPhysics::UnfreezeObjectPhysicsImpl
{
private:

	std::weak_ptr<GetObjectAddress> getObjectAddressWeak;

	enum class bipedDataFields { currentVehicleDatum };
	std::shared_ptr<DynamicStruct<bipedDataFields>> bipedDataStruct;

	enum class physicsObjectDataFields { freezeFlag };
	std::shared_ptr<DynamicStruct<physicsObjectDataFields>> physicsObjectDataStruct;

	int64_t freezeFlagBit;

public:
	UnfreezeObjectPhysicsImpl(GameState game, IDIContainer& dicon)
		:
		getObjectAddressWeak(resolveDependentCheat(GetObjectAddress))
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		bipedDataStruct = DynamicStructFactory::make<bipedDataFields>(ptr, game);
		physicsObjectDataStruct = DynamicStructFactory::make<physicsObjectDataFields>(ptr, game);
		freezeFlagBit = *ptr->getData<std::shared_ptr<int64_t>>(nameof(freezeFlagBit), game).get();
	}

	void unfreezeObjectPhysics(Datum datumToUnfreeze)
	{

		lockOrThrow(getObjectAddressWeak, getObjectAddress);

		CommonObjectType commonObjectType;
		auto physicsObjectAddress = getObjectAddress->getObjectAddress(datumToUnfreeze, &commonObjectType);

		if (commonObjectType == CommonObjectType::Biped)
		{
			// get bipeds vehicle datum
			bipedDataStruct->currentBaseAddress = physicsObjectAddress;
			Datum bipedsVehicleDatum = *bipedDataStruct->field<Datum>(bipedDataFields::currentVehicleDatum);

			if (!bipedsVehicleDatum.isNull())
			{
				// promote biped to vehicle
				physicsObjectAddress = getObjectAddress->getObjectAddress(datumToUnfreeze, &commonObjectType);
			}

		}

		// check that the commonObjectType is one that is capable of having it's physics unfrozen
		if (commonObjectType != CommonObjectType::Biped
			&& commonObjectType != CommonObjectType::Vehicle
			&& commonObjectType != CommonObjectType::Scenery
			&& commonObjectType != CommonObjectType::Weapon
			&& commonObjectType != CommonObjectType::Equipment
			)
		{
			throw HCMRuntimeException(std::format("Cannot unfreeze physics of object of type {}, datum {}", magic_enum::enum_name(commonObjectType), datumToUnfreeze.toString()));
		}

		physicsObjectDataStruct->currentBaseAddress = physicsObjectAddress;

		auto* freezeFlagByte = physicsObjectDataStruct->field<byte>(physicsObjectDataFields::freezeFlag);
		// set bit flag to false

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "Setting freeze flag to false at address: " << addy, addy = (uintptr_t)freezeFlagByte);

		*freezeFlagByte = *freezeFlagByte & ~(1 << freezeFlagBit);

	}
};



UnfreezeObjectPhysics::UnfreezeObjectPhysics(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<UnfreezeObjectPhysicsImpl>(gameImpl, dicon);
}

UnfreezeObjectPhysics::~UnfreezeObjectPhysics()
{
	PLOG_DEBUG << "~" << getName();
}

void UnfreezeObjectPhysics::unfreezeObjectPhysics(Datum datumToUnfreeze) { return pimpl->unfreezeObjectPhysics(datumToUnfreeze); }