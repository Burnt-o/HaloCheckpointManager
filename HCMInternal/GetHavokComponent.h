#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "Datum.h"
#include "ObjectTypes.h"

class GetHavokComponent : public IOptionalCheat
{
public:
	class IGetHavokComponentImpl
	{
	public:
		virtual ~IGetHavokComponentImpl() = default;
		virtual uintptr_t getHavokComponent(Datum havokDatum) = 0;
	};
private:
	std::unique_ptr<IGetHavokComponentImpl> pimpl;

public:
	GetHavokComponent(GameState game, IDIContainer& dicon);
	~GetHavokComponent();


	uintptr_t getHavokComponent(Datum havokDatum);

	virtual std::string_view getName() override { return nameof(GetHavokComponent); }
};
