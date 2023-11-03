#include "pch.h"
#include "GetObjectPhysics.h"
#include "GetObjectAddress.h"
#include "DynamicStructFactory.h"
#include "ObjectTypes.h"
#include "IMakeOrGetCheat.h"
#include "GetHavokAnchorPoint.h"

class GetObjectPhysicsH1 : public IGetObjectPhysics
{
private:



	// injected services
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;

	//data
	enum class physicsObjectDataFields { position, velocity };
	std::shared_ptr<DynamicStruct<physicsObjectDataFields>> physicsObjectDataStruct;

	enum class bipedDataFields { currentVehicleDatum};
	std::shared_ptr<DynamicStruct<bipedDataFields>> bipedDataStruct;
public:
	GetObjectPhysicsH1(GameState game, IDIContainer& dicon)
	{
		getObjectAddressWeak = resolveDependentCheat(GetObjectAddress);

		auto ptr = dicon.Resolve<PointerManager>().lock();
		physicsObjectDataStruct = DynamicStructFactory::make<physicsObjectDataFields>(ptr, game);
		bipedDataStruct = DynamicStructFactory::make<bipedDataFields>(ptr, game);
	}


	virtual const SimpleMath::Vector3*getObjectPosition(Datum datum) override
	{
		return getObjectPositionMutableAndVisual(datum).first;
	}
	virtual const SimpleMath::Vector3* getObjectVelocity(Datum datum) override
	{
		return getObjectVelocityMutable(datum);
	}
	virtual std::pair<SimpleMath::Vector3*, SimpleMath::Vector3*> getObjectPositionMutableAndVisual(Datum datum) override
	{

		if (datum.isNull()) throw HCMRuntimeException("getObjectPositionMutable:: Null datum!");

		lockOrThrow(getObjectAddressWeak, getObjectAddress);

		CommonObjectType thisObjectTypeEnum;
		auto physicsObjectAddress = getObjectAddress->getObjectAddress(datum, &thisObjectTypeEnum);
	
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "running on object type: " << type, type = magic_enum::enum_name(thisObjectTypeEnum));

		if (thisObjectTypeEnum == CommonObjectType::Biped)
		{
			LOG_ONCE(PLOG_DEBUG << "Object is biped: checking if in vehicle");
			// get bipeds vehicle datum
			bipedDataStruct->currentBaseAddress = physicsObjectAddress;
			Datum bipedsVehicleDatum = *bipedDataStruct->field<Datum>(bipedDataFields::currentVehicleDatum);

			if (!bipedsVehicleDatum.isNull())
			{
				LOG_ONCE(PLOG_DEBUG << "Promoting to vehicles datum");
				// promote biped to vehicle
				physicsObjectAddress = getObjectAddress->getObjectAddress(datum, &thisObjectTypeEnum);
				if (thisObjectTypeEnum != CommonObjectType::Vehicle) throw HCMRuntimeException(std::format("bipeds Vehicle somehow wasn't a vehicle! Was {} instead", magic_enum::enum_name(thisObjectTypeEnum)));
			}
		}	

		// check that the commonObjectType is one that has a position/velocity field
		if (thisObjectTypeEnum != CommonObjectType::Biped
			&& thisObjectTypeEnum != CommonObjectType::Vehicle
			&& thisObjectTypeEnum != CommonObjectType::Scenery
			&& thisObjectTypeEnum != CommonObjectType::Weapon
			&& thisObjectTypeEnum != CommonObjectType::Equipment
			)
		{
			throw HCMRuntimeException(std::format("Cannot get physics of object of type {}, datum {}", magic_enum::enum_name(thisObjectTypeEnum), datum.toString()));
		}

		physicsObjectDataStruct->currentBaseAddress = physicsObjectAddress;

		auto* outVisualAndRealPos = physicsObjectDataStruct->field<SimpleMath::Vector3>(physicsObjectDataFields::position);

		return { outVisualAndRealPos , outVisualAndRealPos };

	}

	virtual SimpleMath::Vector3* getObjectVelocityMutable(Datum datum) override
	{
		if (datum.isNull()) throw HCMRuntimeException("getObjectPositionMutable:: Null datum!");

		lockOrThrow(getObjectAddressWeak, getObjectAddress);

		CommonObjectType thisObjectTypeEnum;
		auto physicsObjectAddress = getObjectAddress->getObjectAddress(datum, &thisObjectTypeEnum);

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "running on object type: " << type, type = magic_enum::enum_name(thisObjectTypeEnum));

		if (thisObjectTypeEnum == CommonObjectType::Biped)
		{
			LOG_ONCE(PLOG_DEBUG << "Object is biped: checking if in vehicle");
			// get bipeds vehicle datum
			bipedDataStruct->currentBaseAddress = physicsObjectAddress;
			Datum bipedsVehicleDatum = *bipedDataStruct->field<Datum>(bipedDataFields::currentVehicleDatum);

			if (!bipedsVehicleDatum.isNull())
			{
				LOG_ONCE(PLOG_DEBUG << "Promoting to vehicles datum");
				// promote biped to vehicle
				physicsObjectAddress = getObjectAddress->getObjectAddress(datum, &thisObjectTypeEnum);
				if (thisObjectTypeEnum != CommonObjectType::Vehicle) throw HCMRuntimeException(std::format("bipeds Vehicle somehow wasn't a vehicle! Was {} instead", magic_enum::enum_name(thisObjectTypeEnum)));
			}
		}

		// check that the commonObjectType is one that has a position/velocity field
		if (thisObjectTypeEnum != CommonObjectType::Biped
			&& thisObjectTypeEnum != CommonObjectType::Vehicle
			&& thisObjectTypeEnum != CommonObjectType::Scenery
			&& thisObjectTypeEnum != CommonObjectType::Weapon
			&& thisObjectTypeEnum != CommonObjectType::Equipment
			)
		{
			throw HCMRuntimeException(std::format("Cannot get physics of object of type {}, datum {}", magic_enum::enum_name(thisObjectTypeEnum), datum.toString()));
		}

		physicsObjectDataStruct->currentBaseAddress = physicsObjectAddress;

		return physicsObjectDataStruct->field<SimpleMath::Vector3>(physicsObjectDataFields::velocity);
	}
};


class GetObjectPhysicsHavokVisualHavok : public IGetObjectPhysics
{
private:

	// injected services
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;
	std::weak_ptr< GetHavokAnchorPoint> getHavokAnchorPointWeak;

	//data
	enum class physicsObjectDataFields { havokComponentDatum };
	std::shared_ptr<DynamicStruct<physicsObjectDataFields>> physicsObjectDataStruct;

	enum class bipedDataFields { currentVehicleDatum };
	std::shared_ptr<DynamicStruct<bipedDataFields>> bipedDataStruct;

	enum class havokComponentDataFields { position, visualPosition, velocity };
	std::shared_ptr<DynamicStruct<havokComponentDataFields>> havokAnchorPointDataStruct;
public:
	GetObjectPhysicsHavokVisualHavok(GameState game, IDIContainer& dicon)
	{
		getObjectAddressWeak = resolveDependentCheat(GetObjectAddress);
		getHavokAnchorPointWeak = resolveDependentCheat(GetHavokAnchorPoint);

		auto ptr = dicon.Resolve<PointerManager>().lock();
		physicsObjectDataStruct = DynamicStructFactory::make<physicsObjectDataFields>(ptr, game);
		havokAnchorPointDataStruct = DynamicStructFactory::make<havokComponentDataFields>(ptr, game);
		bipedDataStruct = DynamicStructFactory::make<bipedDataFields>(ptr, game);
	}


	virtual const SimpleMath::Vector3* getObjectPosition(Datum datum) override
	{
		return getObjectPositionMutableAndVisual(datum).first;
	}
	virtual const SimpleMath::Vector3* getObjectVelocity(Datum datum) override
	{
		return getObjectVelocityMutable(datum);
	}
	virtual std::pair<SimpleMath::Vector3*, SimpleMath::Vector3*> getObjectPositionMutableAndVisual(Datum datum) override
	{
		if (datum.isNull()) throw HCMRuntimeException("getObjectPositionMutable:: Null datum!");

		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		lockOrThrow(getHavokAnchorPointWeak, getHavokAnchorPoint);

		CommonObjectType thisObjectTypeEnum;
		auto objectAddress = getObjectAddress->getObjectAddress(datum, &thisObjectTypeEnum);
		physicsObjectDataStruct->currentBaseAddress = objectAddress;

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "running on object type: " << type, type = magic_enum::enum_name(thisObjectTypeEnum));

		if (thisObjectTypeEnum == CommonObjectType::Biped)
		{
			LOG_ONCE(PLOG_DEBUG << "Object is biped: checking if in vehicle");
			// need to check if the biped is in a vehicle- if so, we want to return the vehicles data instead of the bipeds
			bipedDataStruct->currentBaseAddress = objectAddress;
			auto* bipedsVehicleDatum = bipedDataStruct->field<Datum>(bipedDataFields::currentVehicleDatum);
			if (!bipedsVehicleDatum->isNull())
			{
				LOG_ONCE(PLOG_DEBUG << "Promoting to vehicles datum");
				return getObjectPositionMutableAndVisual(*bipedsVehicleDatum);
				//objectAddress = getObjectAddress->getObjectAddress(*bipedsVehicleDatum, &thisObjectTypeEnum);
				//if (thisObjectTypeEnum != CommonObjectType::Vehicle) throw HCMRuntimeException(std::format("bipeds Vehicle somehow wasn't a vehicle! Was {} instead", magic_enum::enum_name(thisObjectTypeEnum)));
			}
		}

		auto* havokComponentDatum = physicsObjectDataStruct->field<Datum>(physicsObjectDataFields::havokComponentDatum);
		
		auto havokAnchorPointAddress = getHavokAnchorPoint->getHavokAnchorPoint(*havokComponentDatum);

		havokAnchorPointDataStruct->currentBaseAddress = havokAnchorPointAddress;

		auto* outRealPos = havokAnchorPointDataStruct->field<SimpleMath::Vector3>(havokComponentDataFields::position);
		auto* outVisPos = havokAnchorPointDataStruct->field<SimpleMath::Vector3>(havokComponentDataFields::visualPosition);

		return { outRealPos , outVisPos };

	}

	virtual SimpleMath::Vector3* getObjectVelocityMutable(Datum datum) override
	{

		if (datum.isNull()) throw HCMRuntimeException("getObjectVelocityMutable:: Null datum!");

		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		lockOrThrow(getHavokAnchorPointWeak, getHavokAnchorPoint);

		CommonObjectType thisObjectTypeEnum;
		auto objectAddress = getObjectAddress->getObjectAddress(datum, &thisObjectTypeEnum);
		physicsObjectDataStruct->currentBaseAddress = objectAddress;

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "running on object type: " << type, type = magic_enum::enum_name(thisObjectTypeEnum));


		if (thisObjectTypeEnum == CommonObjectType::Biped)
		{
			LOG_ONCE(PLOG_DEBUG << "Object is biped: checking if in vehicle");
			// need to check if the biped is in a vehicle- if so, we want to return the vehicles data instead of the bipeds
			bipedDataStruct->currentBaseAddress = objectAddress;
			auto* bipedsVehicleDatum = bipedDataStruct->field<Datum>(bipedDataFields::currentVehicleDatum);
			if (!bipedsVehicleDatum->isNull())
			{
				LOG_ONCE(PLOG_DEBUG << "Promoting to vehicles datum");
				return getObjectVelocityMutable(*bipedsVehicleDatum);
				//objectAddress = getObjectAddress->getObjectAddress(*bipedsVehicleDatum, &thisObjectTypeEnum);
				//if (thisObjectTypeEnum != CommonObjectType::Vehicle) throw HCMRuntimeException(std::format("bipeds Vehicle somehow wasn't a vehicle! Was {} instead", magic_enum::enum_name(thisObjectTypeEnum)));
			}
		}

		auto* havokComponentDatum = physicsObjectDataStruct->field<Datum>(physicsObjectDataFields::havokComponentDatum);

		auto havokAnchorPointAddress = getHavokAnchorPoint->getHavokAnchorPoint(*havokComponentDatum);

		havokAnchorPointDataStruct->currentBaseAddress = havokAnchorPointAddress;

		return havokAnchorPointDataStruct->field<SimpleMath::Vector3>(havokComponentDataFields::velocity);
	}
};





class GetObjectPhysicsHavokVisualObject : public IGetObjectPhysics
{
private:

	// injected services
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;
	std::weak_ptr< GetHavokAnchorPoint> getHavokAnchorPointWeak;

	//data
	enum class physicsObjectDataFields { havokComponentDatum, visualPosition };
	std::shared_ptr<DynamicStruct<physicsObjectDataFields>> physicsObjectDataStruct;

	enum class bipedDataFields { currentVehicleDatum };
	std::shared_ptr<DynamicStruct<bipedDataFields>> bipedDataStruct;

	enum class havokComponentDataFields { position, velocity };
	std::shared_ptr<DynamicStruct<havokComponentDataFields>> havokAnchorPointDataStruct;
public:
	GetObjectPhysicsHavokVisualObject(GameState game, IDIContainer& dicon)
	{
		getObjectAddressWeak = resolveDependentCheat(GetObjectAddress);
		getHavokAnchorPointWeak = resolveDependentCheat(GetHavokAnchorPoint);

		auto ptr = dicon.Resolve<PointerManager>().lock();
		physicsObjectDataStruct = DynamicStructFactory::make<physicsObjectDataFields>(ptr, game);
		havokAnchorPointDataStruct = DynamicStructFactory::make<havokComponentDataFields>(ptr, game);
		bipedDataStruct = DynamicStructFactory::make<bipedDataFields>(ptr, game);
	}


	virtual const SimpleMath::Vector3* getObjectPosition(Datum datum) override
	{
		return getObjectPositionMutableAndVisual(datum).first;
	}
	virtual const SimpleMath::Vector3* getObjectVelocity(Datum datum) override
	{
		return getObjectVelocityMutable(datum);
	}
	virtual std::pair<SimpleMath::Vector3*, SimpleMath::Vector3*> getObjectPositionMutableAndVisual(Datum datum) override
	{
		if (datum.isNull()) throw HCMRuntimeException("getObjectPositionMutable:: Null datum!");


		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		lockOrThrow(getHavokAnchorPointWeak, getHavokAnchorPoint);

		CommonObjectType thisObjectTypeEnum;
		auto objectAddress = getObjectAddress->getObjectAddress(datum, &thisObjectTypeEnum);
		physicsObjectDataStruct->currentBaseAddress = objectAddress;

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "running on object type: " << type, type = magic_enum::enum_name(thisObjectTypeEnum));
		if (thisObjectTypeEnum == CommonObjectType::Biped)
		{
			LOG_ONCE(PLOG_DEBUG << "Object is biped: checking if in vehicle");
			// need to check if the biped is in a vehicle- if so, we want to return the vehicles data instead of the bipeds
			bipedDataStruct->currentBaseAddress = objectAddress;
			auto* bipedsVehicleDatum = bipedDataStruct->field<Datum>(bipedDataFields::currentVehicleDatum);
			if (!bipedsVehicleDatum->isNull())
			{
				LOG_ONCE(PLOG_DEBUG << "Promoting to vehicles datum");
				return getObjectPositionMutableAndVisual(*bipedsVehicleDatum);
				//objectAddress = getObjectAddress->getObjectAddress(*bipedsVehicleDatum, &thisObjectTypeEnum);
				//if (thisObjectTypeEnum != CommonObjectType::Vehicle) throw HCMRuntimeException(std::format("bipeds Vehicle somehow wasn't a vehicle! Was {} instead", magic_enum::enum_name(thisObjectTypeEnum)));
			}
		}

		auto* havokComponentDatum = physicsObjectDataStruct->field<Datum>(physicsObjectDataFields::havokComponentDatum);

		auto havokAnchorPointAddress = getHavokAnchorPoint->getHavokAnchorPoint(*havokComponentDatum);

		havokAnchorPointDataStruct->currentBaseAddress = havokAnchorPointAddress;


		auto* outRealPos = havokAnchorPointDataStruct->field<SimpleMath::Vector3>(havokComponentDataFields::position);
		auto* outVisPos = physicsObjectDataStruct->field<SimpleMath::Vector3>(physicsObjectDataFields::visualPosition);

		return { outRealPos , outVisPos };

	}

	virtual SimpleMath::Vector3* getObjectVelocityMutable(Datum datum) override
	{

		if (datum.isNull()) throw HCMRuntimeException("getObjectVelocityMutable:: Null datum!");

		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		lockOrThrow(getHavokAnchorPointWeak, getHavokAnchorPoint);

		CommonObjectType thisObjectTypeEnum;
		auto objectAddress = getObjectAddress->getObjectAddress(datum, &thisObjectTypeEnum);
		physicsObjectDataStruct->currentBaseAddress = objectAddress;

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "running on object type: " << type, type = magic_enum::enum_name(thisObjectTypeEnum));

		if (thisObjectTypeEnum == CommonObjectType::Biped)
		{
			LOG_ONCE(PLOG_DEBUG << "Object is biped: checking if in vehicle");
			// need to check if the biped is in a vehicle- if so, we want to return the vehicles data instead of the bipeds
			bipedDataStruct->currentBaseAddress = objectAddress;
			auto* bipedsVehicleDatum = bipedDataStruct->field<Datum>(bipedDataFields::currentVehicleDatum);
			if (!bipedsVehicleDatum->isNull())
			{
				LOG_ONCE(PLOG_DEBUG << "Promoting to vehicles datum");
				return getObjectVelocityMutable(*bipedsVehicleDatum);
				//objectAddress = getObjectAddress->getObjectAddress(*bipedsVehicleDatum, &thisObjectTypeEnum);
				//if (thisObjectTypeEnum != CommonObjectType::Vehicle) throw HCMRuntimeException(std::format("bipeds Vehicle somehow wasn't a vehicle! Was {} instead", magic_enum::enum_name(thisObjectTypeEnum)));
			}
		}

		auto* havokComponentDatum = physicsObjectDataStruct->field<Datum>(physicsObjectDataFields::havokComponentDatum);

		auto havokAnchorPointAddress = getHavokAnchorPoint->getHavokAnchorPoint(*havokComponentDatum);

		havokAnchorPointDataStruct->currentBaseAddress = havokAnchorPointAddress;

		return havokAnchorPointDataStruct->field<SimpleMath::Vector3>(havokComponentDataFields::velocity);
	}
};






GetObjectPhysics::GetObjectPhysics(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: pimpl = std::make_unique<GetObjectPhysicsH1>(game, dicon); break;
	case GameState::Value::Halo2: pimpl = std::make_unique<GetObjectPhysicsHavokVisualHavok>(game, dicon); break;
	case GameState::Value::Halo3: pimpl = std::make_unique<GetObjectPhysicsHavokVisualObject>(game, dicon); break;
	case GameState::Value::Halo3ODST: pimpl = std::make_unique<GetObjectPhysicsHavokVisualObject>(game, dicon); break;
	case GameState::Value::HaloReach: pimpl = std::make_unique<GetObjectPhysicsHavokVisualObject>(game, dicon); break;
	case GameState::Value::Halo4: pimpl = std::make_unique<GetObjectPhysicsHavokVisualObject>(game, dicon); break;
	}
}

GetObjectPhysics::~GetObjectPhysics() { PLOG_DEBUG << "~" << getName(); }


const SimpleMath::Vector3* GetObjectPhysics::getObjectPosition(Datum datum)
{
	return pimpl->getObjectPosition(datum);
}
std::pair<SimpleMath::Vector3*, SimpleMath::Vector3*> GetObjectPhysics::getObjectPositionMutableAndVisual(Datum datum)
{
	return pimpl->getObjectPositionMutableAndVisual(datum);
}
const SimpleMath::Vector3* GetObjectPhysics::getObjectVelocity(Datum datum)
{
	return pimpl->getObjectVelocity(datum);
}
SimpleMath::Vector3* GetObjectPhysics::getObjectVelocityMutable(Datum datum)
{
	return pimpl->getObjectVelocityMutable(datum);
}