#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "TriggerData.h"

#include "CsLibGuarded\cs_plain_guarded.h"

typedef uintptr_t TriggerPointer;
typedef std::map<TriggerPointer, TriggerData> TriggerDataMap;
typedef libguarded::plain_guarded<TriggerDataMap>::handle TriggerDataMapLock;






class GetTriggerDataUntemplated
{

public:
	virtual ~GetTriggerDataUntemplated() = default;
	virtual TriggerDataMapLock getAllTriggers() = 0;
	virtual TriggerDataMapLock getFilteredTriggers() = 0;
	virtual std::shared_ptr<eventpp::CallbackList<void(void)>> getTriggerDataChangedEvent() = 0;

};

class GetTriggerData : public IOptionalCheat
{
private:
	std::unique_ptr<GetTriggerDataUntemplated> pimpl;
public:

	GetTriggerData(GameState gameImpl, IDIContainer& dicon);
	~GetTriggerData();

	TriggerDataMapLock getAllTriggers();
	TriggerDataMapLock getFilteredTriggers();

	std::shared_ptr<eventpp::CallbackList<void(void)>> getTriggerDataChangedEvent() { return pimpl->getTriggerDataChangedEvent(); }

	std::string_view getName() override { return nameof(GetTriggerData); }




};