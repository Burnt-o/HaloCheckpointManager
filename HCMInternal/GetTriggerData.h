#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "TriggerData.h"

typedef uintptr_t TriggerPointer;
typedef std::map<TriggerPointer, TriggerData> TriggerDataMap;


struct IAtomicTriggerDataStore
{
private:
	friend class TriggerFilterImpl;
	std::shared_ptr<TriggerDataMap> allTriggers;

protected:
	IAtomicTriggerDataStore(
		std::shared_ptr<TriggerDataMap> allT,
		std::shared_ptr<TriggerDataMap> filteredT)
		:
		allTriggers(allT), 
		filteredTriggers(filteredT) 
	{}



public:
	virtual ~IAtomicTriggerDataStore() = default;

	std::shared_ptr<TriggerDataMap> filteredTriggers;
};

class GetTriggerDataUntemplated
{
protected:
	// friendship proxy
	TriggerData makeTriggerData(std::string triggerName, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
	{
		return std::move(TriggerData(triggerName, position, extents, forward, up));
	}

public:
	virtual ~GetTriggerDataUntemplated() = default;
	virtual std::unique_ptr<IAtomicTriggerDataStore> getTriggerData() = 0;

};

class GetTriggerData : public IOptionalCheat
{

public:

	GetTriggerData(GameState gameImpl, IDIContainer& dicon);
	~GetTriggerData();

	std::unique_ptr<IAtomicTriggerDataStore> getTriggerData();

	std::string_view getName() override { return nameof(GetTriggerData); }

private:
	std::unique_ptr<GetTriggerDataUntemplated> pimpl;

};