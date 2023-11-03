#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "Datum.h"
#include "ObjectTypes.h"




class GetHavokAnchorPoint : public IOptionalCheat
{
public:
	class IGetHavokAnchorPointImpl
	{
	public:
		virtual ~IGetHavokAnchorPointImpl() = default;
		virtual uintptr_t getHavokAnchorPoint(Datum havokDatum) = 0;
		virtual uintptr_t getHavokAnchorPoint(uintptr_t havokComponentAddress) = 0;
	};
private:
	std::unique_ptr<IGetHavokAnchorPointImpl> pimpl;

public:
	GetHavokAnchorPoint(GameState game, IDIContainer& dicon);
	~GetHavokAnchorPoint();


	uintptr_t getHavokAnchorPoint(Datum havokDatum);

	uintptr_t getHavokAnchorPoint(uintptr_t havokComponentAddress);

	virtual std::string_view getName() override { return nameof(GetHavokAnchorPoint); }
};
