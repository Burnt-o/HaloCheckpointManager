#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "Datum.h"


class GetObjectTagName : public IOptionalCheat
{
private:
	class GetObjectTagNameImpl;
	std::unique_ptr<GetObjectTagNameImpl> pimpl;

public:
	GetObjectTagName(GameState gameImpl, IDIContainer& dicon);
	~GetObjectTagName();

	const char* getObjectTagName(Datum objectDatum);

	std::string_view getName() override { return nameof(GetObjectTagName); }

};