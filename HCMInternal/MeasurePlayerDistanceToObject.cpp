#include "pch.h"
#include "MeasurePlayerDistanceToObject.h"
#include "GetPlayerDatum.h"
#include "GetObjectAddress.h"
#include "GetObjectPhysics.h"
#include "IMakeOrGetCheat.h"

class MeasurePlayerDistanceToObject::MeasurePlayerDistanceToObjectImpl
{
private:


	// injected services
	std::weak_ptr< GetPlayerDatum> getPlayerDatumWeak;
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;
	std::weak_ptr< GetObjectPhysics> getObjectPhysicsWeak;



public:
	std::optional<float> measure(SimpleMath::Vector3 worldPos)
	{
		try
		{
			lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
			auto playerDatum = getPlayerDatum->getPlayerDatum();
			if (playerDatum.isNull()) return std::nullopt;

			lockOrThrow(getObjectAddressWeak, getObjectAddress);
			if (getObjectAddress->isValidDatum(playerDatum) == false) return std::nullopt;

			lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);
			auto playerPosition = getObjectPhysics->getObjectPosition(playerDatum);

			return SimpleMath::Vector3::Distance(worldPos, *playerPosition);
		}
		catch (HCMRuntimeException ex)
		{
			PLOG_ERROR << "MeasurePlayerDistanceToObject error: " << ex.what();
			return std::nullopt;
		}

	}
	std::optional<float> measure(Datum objectDatum)
	{
		try
		{
			if (objectDatum.isNull()) return std::nullopt;

			lockOrThrow(getObjectAddressWeak, getObjectAddress);
			if (getObjectAddress->isValidDatum(objectDatum) == false) return std::nullopt;

			lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);
			auto objectPosition = getObjectPhysics->getObjectPosition(objectDatum);

			return measure(*objectPosition);
		}
		catch (HCMRuntimeException ex)
		{
			PLOG_ERROR << "MeasurePlayerDistanceToObject error: " << ex.what();
			return std::nullopt;
		}
	}


	MeasurePlayerDistanceToObjectImpl(GameState game, IDIContainer& dicon)
		: 
		getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum)),
		getObjectAddressWeak(resolveDependentCheat(GetObjectAddress)),
		getObjectPhysicsWeak(resolveDependentCheat(GetObjectPhysics))
	{

	}

};



MeasurePlayerDistanceToObject::MeasurePlayerDistanceToObject(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<MeasurePlayerDistanceToObjectImpl>(gameImpl, dicon);
}
MeasurePlayerDistanceToObject::~MeasurePlayerDistanceToObject()
{
	PLOG_VERBOSE << "~" << getName();
}

std::optional<float> MeasurePlayerDistanceToObject::measure(SimpleMath::Vector3 worldPos) { return pimpl->measure(worldPos); }
std::optional<float> MeasurePlayerDistanceToObject::measure(Datum objectDatum) { return pimpl->measure(objectDatum); }