#pragma once
#include "pch.h"
#include "GameState.h"
#include "GetObjectAddress.h"
#include "GetObjectPhysics.h"
#include "GetObjectHealth.h"
#include "GetPlayerDatum.h"
#include "GetPlayerViewAngle.h"
#include "GetBipedsVehicleDatum.h"
#include "ViewAngleToSubpixelID.h"

//https://stackoverflow.com/a/43482688
struct separate_thousands : std::numpunct<char> {
	char_type do_thousands_sep() const override { return ','; }  // separate with commas
	string_type do_grouping() const override { return "\3"; } // groups of 3 digit
};

template <GameState::Value gameT>
class GetPlayerDataAsString
{
private:
	std::string dataStringA;
	std::string dataStringB;
	std::locale thousandsSeperatedLocale = std::locale(std::cout.getloc(), new separate_thousands);

public:
	std::string_view getDataString(bool useDataStringA) { return useDataStringA ? dataStringA : dataStringB; }

	// setup stringstream formatting flags and preallocate some memory
	std::ostringstream ss = (static_cast<std::ostringstream&&>(std::ostringstream{} << std::setprecision(6) << std::fixed << std::showpos << std::string(200, '\0')));
	void updatePlayerData(bool useDataStringA)
	{
		ss.str("");


		ss << "Player Data: " << std::endl;

		lockOrThrow(getObjectAddressWeak, getObjectAddress);

		LOG_ONCE(PLOG_VERBOSE << "GetPlayerDataAsString::updatePlayerData");

		lockOrThrow(getPlayerDatumWeak, getPlayerDatum)
		Datum playerDatum = getPlayerDatum->getPlayerDatum();

		if (getObjectAddress->isValidDatum(playerDatum))
		{
			CommonObjectType playerObjectType;
			auto playerAddress = getObjectAddress->getObjectAddress(playerDatum, &playerObjectType);

			if (getPlayerViewAngleOptionalWeak.has_value())
			{
				lockOrThrow(getPlayerViewAngleOptionalWeak.value(), getPlayerViewAngle);
				const auto currentViewAngle = getPlayerViewAngle->getPlayerViewAngle();
				ss << " LookAngle: " << currentViewAngle << std::endl;
			}

			if (getPlayerViewAngleIDOptionalWeak.has_value())
			{
				lockOrThrow(getPlayerViewAngleIDOptionalWeak.value(), getPlayerViewAngle);
				const auto currentViewAngle = getPlayerViewAngle->getPlayerViewAngle();

				// get thousands seperator to make number easier to read
				auto prevLocale = ss.imbue(thousandsSeperatedLocale);
				ss << " Subpixel ID: " << ViewAngleToSubpixelID(currentViewAngle.x) << std::endl;
				ss.imbue(prevLocale); // pop locale

			}

			if (getObjectPhysicsOptionalWeakPos.has_value())
			{
				lockOrThrow(getObjectPhysicsOptionalWeakPos.value(), getObjectPhysics);
				const auto* currentPosition = getObjectPhysics->getObjectPosition(playerDatum);
				ss << " Pos: " << *currentPosition << std::endl;
			}

			if (playerObjectType == CommonObjectType::Biped || playerObjectType == CommonObjectType::Vehicle)
			{

				if (getObjectPhysicsOptionalWeakVel.has_value())
				{
					lockOrThrow(getObjectPhysicsOptionalWeakVel.value(), getObjectPhysics);
					const auto* currentVelocity = getObjectPhysics->getObjectVelocity(playerDatum);

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
					ss << std::noshowpos;

					lockOrThrow(getObjectHealthOptionalWeak.value(), getObjectHealth);
					auto health = getObjectHealth->getObjectHealth(playerAddress, playerObjectType);
					ss << " Health: " << health.value << std::endl;


					if constexpr (gameT == GameState::Value::HaloReach || gameT == GameState::Value::Halo4)
					{
						if (playerObjectType == CommonObjectType::Biped && showHealthShieldCooldown)
						{
							auto healthCooldown = getObjectHealth->getObjectHealthCooldown(playerAddress, playerObjectType);

							constexpr float gameTickRate = gameT == GameState::Value::Halo1 ? 30.f : 60.f;

							float healthCooldownInSeconds = healthCooldown == 0 ? 0.f : ((float)healthCooldown / gameTickRate);
							ss << "  Cooldown: " << healthCooldownInSeconds << "s" << std::endl;
						}
					}


					if (playerObjectType == CommonObjectType::Biped)
					{
						auto shields = getObjectHealth->getObjectShields(playerAddress, playerObjectType);
						ss << " Shield: " << shields.value << std::endl;
					}

					if (playerObjectType == CommonObjectType::Biped && showHealthShieldCooldown)
					{
						auto shieldCooldown = getObjectHealth->getObjectShieldCooldown(playerAddress, playerObjectType);

						constexpr float gameTickRate = gameT == GameState::Value::Halo1 ? 30.f : 60.f;

						float shieldCooldownInSeconds = shieldCooldown == 0 ? 0.f : ((float)shieldCooldown / gameTickRate);
						ss << "  Cooldown: " << shieldCooldownInSeconds << "s" << std::endl;
					}


					if (getBipedsVehicleDatumOptionalWeak.has_value() && playerObjectType == CommonObjectType::Biped)
					{
						lockOrThrow(getBipedsVehicleDatumOptionalWeak.value(), getBipedsVehicleDatum);

						auto vehicleDatum = getBipedsVehicleDatum->getBipedsVehicleDatum(playerAddress);

						if (!vehicleDatum.isNull() && getObjectAddress->isValidDatum(vehicleDatum))
						{
							auto vehiHealth = getObjectHealth->getObjectHealth(vehicleDatum);
							ss << " Vehi Health: " << vehiHealth.value << std::endl;
						}

					}

					ss << std::showpos;

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

	GetPlayerDataAsString(std::weak_ptr<GetObjectAddress> getObjectAddress, std::weak_ptr<GetPlayerDatum> getPlayerDatum)
		: getObjectAddressWeak(getObjectAddress), getPlayerDatumWeak(getPlayerDatum)
	{

	}


	// necessary injected services
	std::weak_ptr<GetObjectAddress> getObjectAddressWeak;
	std::weak_ptr<GetPlayerDatum> getPlayerDatumWeak;


	// optional injected services
	std::optional<std::weak_ptr<GetObjectPhysics>> getObjectPhysicsOptionalWeakPos; // getObjectPhysics duplicated for control purposes (pos vs vel)
	std::optional<std::weak_ptr<GetObjectPhysics>> getObjectPhysicsOptionalWeakVel;
	std::optional<std::weak_ptr<GetPlayerViewAngle>> getPlayerViewAngleOptionalWeak;
	std::optional<std::weak_ptr<GetPlayerViewAngle>> getPlayerViewAngleIDOptionalWeak;
	std::optional<std::weak_ptr<GetObjectHealth>> getObjectHealthOptionalWeak; 
	std::optional < std::weak_ptr<GetBipedsVehicleDatum>> getBipedsVehicleDatumOptionalWeak;

	bool velocityAbsolute;
	bool velocityXY;
	bool velocityXYZ;
	bool showHealthShieldCooldown;


};
