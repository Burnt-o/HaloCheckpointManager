#pragma once
#include "ObjectTypes.h"
#include "Datum.h"

class IGameObject
{
public:
	virtual SimpleMath::Vector3 getCenterPosition() = 0;
	virtual float getHealth() = 0;
	virtual float getShields() = 0;
	virtual ~IGameObject() = default;
};

class IGameObjectFactory
{
public:
	virtual std::shared_ptr<IGameObject> makeObject(uint32_t objectOffset) = 0;
	// TODO: extra members for makeObjectAsUnit etc
	virtual ~IGameObjectFactory() = default;
};

struct H1GameObjectMeta : IGameObjectMeta
{
private:
	// data members
	CommonObjectType objectType;
	Datum objectDatum;
	uint32_t objectOffset;
public:
	// accessors
	virtual const CommonObjectType& objectType() override { return objectType; };
	virtual const Datum& objectDatum() override { return objectDatum; }

	// TODO: how do clients inject the factory?
	virtual std::shared_ptr<IGameObject> getObject(std::shared_ptr<IGameObjectFactory> factory) {
		return factory->makeObject(objectOffset);
	}

};

class IGameObjectMeta
{
	/*
	maybe should be dataless ? impls are just the REAL struct
	*/


	virtual const CommonObjectType* objectType() = 0;
	virtual const Datum* objectDatum() = 0;

	virtual std::shared_ptr<IGameObject> getObject() = 0;
};


class IGameObjectMetaFactory
{
public:
	virtual std::shared_ptr<IGameObjectMeta> makeObjectMeta(uint16_t index) = 0;
	~IGameObjectMetaFactory() = default;
};




/*
alternatively.. maybe our object pool iterator is a REAL object meta type, with pointer incrementation etc
using a template !

*/