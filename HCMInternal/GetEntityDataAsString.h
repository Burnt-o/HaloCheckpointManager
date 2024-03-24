#pragma once
#include "pch.h"
#include "GameState.h"
#include "GetObjectAddress.h"
#include "GetObjectPhysics.h"
#include "GetObjectHealth.h"

template <GameState::Value gameT>
class GetEntityDataAsString // mostly designed for ai actors
{
private:
	std::string dataStringA;
	std::string dataStringB;


public:
	// shit.. stringstreams are non contigous so can't have stringview. tho cpp20 may have a workaround? https://stackoverflow.com/questions/47115334/any-way-to-get-an-stdstring-view-from-an-stdostringstream-without-copying
	// std::move ? 
	// well.. appending to a string might not be performance necessary anyway...
	// https://stackoverflow.com/questions/14741144/stdstringstream-vs-stdstring-for-concatenating-many-strings
		// (you can add operator << to std::strings for concatanation)
	// note that you can reserve the expected size of strings

	std::string_view getDataString(bool useDataStringA) { return useDataStringA ? dataStringA : dataStringB; }

	// setup stringstream formatting flags and preallocate some memory
	std::ostringstream ss = (static_cast<std::ostringstream&&>(std::ostringstream{} << std::setprecision(6) << std::fixed << std::showpos << std::string(200, '\0')));
	void updateEntityData(bool useDataStringA)
	{
		ss.str("");

		ss << "Entity Data " << entityDatum << ":" << std::endl;

		lockOrThrow(getObjectAddressWeak, getObjectAddress);

		if (getObjectAddress->isValidDatum(entityDatum))
		{

			CommonObjectType thisEntityType;
			auto entityAddy = getObjectAddress->getObjectAddress(entityDatum, &thisEntityType);

			if (showEntityType)
			{
				ss << " Entity Type: " << magic_enum::enum_name<CommonObjectType>(thisEntityType) << std::endl;
			}

			if (getObjectTagNameOptionalWeak.has_value())
			{
				LOG_ONCE(PLOG_DEBUG << "calling getObjectTagName");
				lockOrThrow(getObjectTagNameOptionalWeak.value(), getObjectTagName);
				ss << " Name: " << getObjectTagName->getObjectTagName(entityDatum) << std::endl;
			}


			if (getObjectPhysicsOptionalWeakPos.has_value())
			{
				lockOrThrow(getObjectPhysicsOptionalWeakPos.value(), getObjectPhysics);
				const auto* currentPosition = getObjectPhysics->getObjectPosition(entityDatum);
				ss << " Pos: " << vec3ToString(*currentPosition) << std::endl;
			}

			if (thisEntityType == CommonObjectType::Biped || thisEntityType == CommonObjectType::Vehicle)
			{

				if (getObjectPhysicsOptionalWeakVel.has_value())
				{
					lockOrThrow(getObjectPhysicsOptionalWeakVel.value(), getObjectPhysics);
					const auto* currentVelocity = getObjectPhysics->getObjectVelocity(entityDatum);

					ss << " Velocity: " << std::endl;
					if (velocityAbsolute)
					{
						ss << "  Abs: " << *currentVelocity << std::endl;
					}

					if (velocityXY)
					{
						float XYMagnitude = std::sqrtf(std::pow(currentVelocity->x, 2) + std::pow(currentVelocity->y, 2));
						ss << "  XY:  " << XYMagnitude << std::endl;
					}

					if (velocityXYZ)
					{
						float XYZMagnitude = std::sqrtf(std::pow(currentVelocity->x, 2) + std::pow(currentVelocity->y, 2) + std::pow(currentVelocity->z, 2));
						ss << "  XYZ: " << XYZMagnitude << std::endl;
					}


				}

				if (getObjectHealthOptionalWeak.has_value())
				{
					lockOrThrow(getObjectHealthOptionalWeak.value(), getObjectHealth);
					auto health = getObjectHealth->getObjectHealth(entityAddy, thisEntityType);
					ss << " Health: " << health.value << std::endl;

					if constexpr (gameT == GameState::Value::HaloReach || gameT == GameState::Value::Halo4)
					{
						if (thisEntityType == CommonObjectType::Biped && showHealthShieldCooldown)
						{
							auto healthCooldown = getObjectHealth->getObjectHealthCooldown(entityAddy, thisEntityType);

							constexpr float gameTickRate = gameT == GameState::Value::Halo1 ? 30.f : 60.f;


							float healthCooldownInSeconds = healthCooldown == 0 ? 0.f : ((float)healthCooldown / gameTickRate);
							ss << "  Cooldown: " << healthCooldownInSeconds << "s" << std::endl;
						}
					}

					if (thisEntityType == CommonObjectType::Biped)
					{
						auto shields = getObjectHealth->getObjectShields(entityAddy, thisEntityType);
						ss << " Shield: " << shields.value << std::endl;
					}


					if (thisEntityType == CommonObjectType::Biped && showHealthShieldCooldown)
					{
						auto shieldCooldown = getObjectHealth->getObjectShieldCooldown(entityAddy, thisEntityType);

						constexpr float gameTickRate = gameT == GameState::Value::Halo1 ? 30.f : 60.f;

						float shieldCooldownInSeconds = shieldCooldown == 0 ? 0.f : ((float)shieldCooldown / gameTickRate);
						ss << "  Cooldown: " << shieldCooldownInSeconds << "s" << std::endl;
					}





					if (getBipedsVehicleDatumOptionalWeak.has_value() && thisEntityType == CommonObjectType::Biped)
					{
						lockOrThrow(getBipedsVehicleDatumOptionalWeak.value(), getBipedsVehicleDatum);

						auto vehicleDatum = getBipedsVehicleDatum->getBipedsVehicleDatum(entityAddy);

						if (!vehicleDatum.isNull() && getObjectAddress->isValidDatum(vehicleDatum))
						{
							auto vehiHealth = getObjectHealth->getObjectHealth(vehicleDatum);
							ss << " Vehi Health: " << vehiHealth.value << std::endl;
						}

					}

				}

			}
		}
		else
		{
			ss << " Invalid Datum!" << std::endl;
		}


	

		if (useDataStringA)
		{
			dataStringA = ss.str();
		}
		else
		{
			dataStringB = ss.str();
		}
	}


	GetEntityDataAsString(std::weak_ptr<GetObjectAddress> getObjectAddress)
		: getObjectAddressWeak(getObjectAddress)
	{

	}


	// necessary injected services
	std::weak_ptr<GetObjectAddress> getObjectAddressWeak;

	// optional injected services
	std::optional<std::weak_ptr<GetObjectPhysics>> getObjectPhysicsOptionalWeakPos = std::nullopt; // getObjectPhysics duplicated for control purposes (pos vs vel)
	std::optional<std::weak_ptr<GetObjectPhysics>> getObjectPhysicsOptionalWeakVel = std::nullopt;
	std::optional<std::weak_ptr<GetObjectHealth>> getObjectHealthOptionalWeak = std::nullopt; // getObjectHealth duplicated for control purposes (main vs vehicle)
	std::optional<std::weak_ptr<GetObjectTagName>> getObjectTagNameOptionalWeak = std::nullopt;
	std::optional<std::weak_ptr<GetBipedsVehicleDatum>> getBipedsVehicleDatumOptionalWeak = std::nullopt;

	// control
	Datum entityDatum;
	bool showEntityType;
	bool velocityAbsolute;
	bool velocityXY;
	bool velocityXYZ;
	bool showHealthShieldCooldown;

};