#include "pch.h"
#include "GetObjectHealth.h"
#include "GetObjectAddress.h"
#include "IMakeOrGetCheat.h"
#include "DynamicStructFactory.h"

class GetObjectHealth::GetObjectHealthImpl
{
private:
	// injected services
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;

	enum class bipedDataFields { health, shields, shieldCooldown };
	std::shared_ptr<DynamicStruct<bipedDataFields>> bipedDataStruct;

	enum class healthCooldownDataFields {  healthCooldown };
	std::optional<std::shared_ptr<DynamicStruct<healthCooldownDataFields>>> healthCooldownDataStruct;

	enum class vehicleDataFields { health };
	std::shared_ptr<DynamicStruct<vehicleDataFields>> vehicleDataStruct;

public:

	GetObjectHealthImpl(GameState game, IDIContainer& dicon)
	{
		getObjectAddressWeak = resolveDependentCheat(GetObjectAddress);
		auto ptr = dicon.Resolve<PointerManager>().lock();
		bipedDataStruct = DynamicStructFactory::make<bipedDataFields>(ptr, game);
		vehicleDataStruct = DynamicStructFactory::make<vehicleDataFields>(ptr, game);


		if (game.operator GameState::Value() == GameState::Value::HaloReach || game.operator GameState::Value() == GameState::Value::Halo4)
		{
				healthCooldownDataStruct = DynamicStructFactory::make<healthCooldownDataFields>(ptr, game);
		}


	}

	float getObjectHealth(Datum entityDatum)
	{
		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		CommonObjectType entityType;
		auto entityAddress = getObjectAddress->getObjectAddress(entityDatum, &entityType);
		return getObjectHealth(entityAddress, entityType);
	}

	float getObjectHealth(uintptr_t entityAddress, CommonObjectType entityType)
	{
		if (entityType == CommonObjectType::Biped)
		{
			bipedDataStruct->currentBaseAddress = entityAddress;
			return *bipedDataStruct->field<float>(bipedDataFields::health);
		}
		else if (entityType == CommonObjectType::Vehicle)
		{
			vehicleDataStruct->currentBaseAddress = entityAddress;
			return *vehicleDataStruct->field<float>(vehicleDataFields::health);
		}
		else
		{
			throw HCMRuntimeException(std::format("Invalid object type passed to getObjectHealth! {}", magic_enum::enum_name<CommonObjectType>(entityType)));
		}
	}

	float getObjectShields(Datum entityDatum)
	{
		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		CommonObjectType entityType;
		auto entityAddress = getObjectAddress->getObjectAddress(entityDatum, &entityType);
		return getObjectHealth(entityAddress, entityType);
	}

	float getObjectShields(uintptr_t entityAddress, CommonObjectType entityType)
	{
		if (entityType == CommonObjectType::Biped)
		{
			bipedDataStruct->currentBaseAddress = entityAddress;
			return *bipedDataStruct->field<float>(bipedDataFields::shields);
		}
		else
		{
			throw HCMRuntimeException(std::format("Invalid object type passed to getObjectShields! {}", magic_enum::enum_name<CommonObjectType>(entityType)));
		}
	}


	uint16_t getObjectShieldCooldown(Datum entityDatum)
	{
		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		CommonObjectType entityType;
		auto entityAddress = getObjectAddress->getObjectAddress(entityDatum, &entityType);
		return getObjectShieldCooldown(entityAddress, entityType);
	}
	uint16_t getObjectShieldCooldown(uintptr_t entityAddress, CommonObjectType entityType) // must be biped
	{
		if (entityType == CommonObjectType::Biped)
		{
			bipedDataStruct->currentBaseAddress = entityAddress;
			return *bipedDataStruct->field<uint16_t>(bipedDataFields::shieldCooldown);
		}
		else
		{
			throw HCMRuntimeException(std::format("Invalid object type passed to getObjectShieldCooldown! {}", magic_enum::enum_name<CommonObjectType>(entityType)));
		}
	}

	uint16_t getObjectHealthCooldown(Datum entityDatum)
	{
		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		CommonObjectType entityType;
		auto entityAddress = getObjectAddress->getObjectAddress(entityDatum, &entityType);
		return getObjectShieldCooldown(entityAddress, entityType);
	}
	uint16_t getObjectHealthCooldown(uintptr_t entityAddress, CommonObjectType entityType) // must be biped
	{
		if (!healthCooldownDataStruct.has_value()) throw HCMRuntimeException("Could not resolve healthCooldownDataStruct");


		if (entityType == CommonObjectType::Biped)
		{
			healthCooldownDataStruct.value()->currentBaseAddress = entityAddress;
			return *healthCooldownDataStruct.value()->field<uint16_t>(healthCooldownDataFields::healthCooldown);
		}
		else
		{
			throw HCMRuntimeException(std::format("Invalid object type passed to getObjectShieldCooldown! {}", magic_enum::enum_name<CommonObjectType>(entityType)));
		}
	}

};




GetObjectHealth::GetObjectHealth(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<GetObjectHealthImpl>(gameImpl, dicon);
}

GetObjectHealth::~GetObjectHealth()
{
	PLOG_DEBUG << "~" << getName();
}


float GetObjectHealth::getObjectHealth(Datum entityDatum)
{
	return pimpl->getObjectHealth(entityDatum);
}
float GetObjectHealth::getObjectHealth(uintptr_t entityAddress, CommonObjectType entityType) // must be biped or vehicle
{
	return pimpl->getObjectHealth(entityAddress, entityType);
}

float GetObjectHealth::getObjectShields(Datum entityDatum)
{
	return pimpl->getObjectShields(entityDatum);
}
float GetObjectHealth::getObjectShields(uintptr_t entityAddress, CommonObjectType entityType) // must be biped or vehicle
{
	return pimpl->getObjectShields(entityAddress, entityType);
}

uint16_t GetObjectHealth::getObjectShieldCooldown(Datum entityDatum)
{
	return pimpl->getObjectShieldCooldown(entityDatum);
}
uint16_t GetObjectHealth::getObjectShieldCooldown(uintptr_t entityAddress, CommonObjectType entityType) // must be biped or vehicle
{
	return pimpl->getObjectShieldCooldown(entityAddress, entityType);
}

uint16_t GetObjectHealth::getObjectHealthCooldown(Datum entityDatum)
{
	return pimpl->getObjectHealthCooldown(entityDatum);
}
uint16_t GetObjectHealth::getObjectHealthCooldown(uintptr_t entityAddress, CommonObjectType entityType) // must be biped or vehicle
{
	return pimpl->getObjectHealthCooldown(entityAddress, entityType);
}