#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "Datum.h"
#include "ObjectTypes.h"


// todo; template by object type
struct ObjectInfo
{
	CommonObjectType objectType;
	uintptr_t objectAddress;
	Datum objectDatum;
};

class IObjectTableRangeImpl {
public:
	virtual ~IObjectTableRangeImpl() = default;
	virtual std::vector<ObjectInfo> getObjectTableRange() = 0;
};

class ObjectTableRange : public IOptionalCheat
{
private:
	std::unique_ptr<IObjectTableRangeImpl> pimpl;

public:
	ObjectTableRange(GameState game, IDIContainer& dicon);
	~ObjectTableRange();


	std::vector<ObjectInfo> getObjectTableRange();

	virtual std::string_view getName() override { return nameof(ObjectTableRange); }
};
