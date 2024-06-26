#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "Datum.h"



class IGetTagAddressImpl
{
public:
	virtual ~IGetTagAddressImpl() = default;
	virtual uintptr_t getTagAddress(Datum tagDatum) = 0;
};

class GetTagAddress : public IOptionalCheat
{
private:
	std::unique_ptr<IGetTagAddressImpl> pimpl;

public:
	GetTagAddress(GameState gameImpl, IDIContainer& dicon);
	~GetTagAddress();

	uintptr_t getTagAddress(Datum tagDatum);


	std::string_view getName() override { return nameof(GetTagAddress); }

};