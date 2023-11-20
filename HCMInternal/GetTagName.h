#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "Datum.h"



class IGetTagNameImpl 
{ 
public: 
	virtual ~IGetTagNameImpl() = default; 
	virtual const char* getTagName(Datum tagDatum) = 0; 
};

class GetTagName : public IOptionalCheat
{
private:
	std::unique_ptr<IGetTagNameImpl> pimpl;

public:
	GetTagName(GameState gameImpl, IDIContainer& dicon);
	~GetTagName();

	const char* getTagName(Datum tagDatum);

	std::string_view getName() override { return nameof(GetTagName); }

};