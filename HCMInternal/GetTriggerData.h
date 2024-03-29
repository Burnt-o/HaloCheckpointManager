#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "TriggerData.h"

typedef uintptr_t TriggerPointer;

class GetTriggerDataUntemplated 
{ 
public: 
	virtual ~GetTriggerDataUntemplated() = default; 
	virtual std::map< TriggerPointer, TriggerData>& getTriggerData() = 0;

};

class GetTriggerData : public IOptionalCheat
{
private:

	std::unique_ptr<GetTriggerDataUntemplated> pimpl;

public:
	GetTriggerData(GameState gameImpl, IDIContainer& dicon);
	~GetTriggerData();

	std::map<TriggerPointer, TriggerData>& getTriggerData();

	std::string_view getName() override { return nameof(GetTriggerData); }

};