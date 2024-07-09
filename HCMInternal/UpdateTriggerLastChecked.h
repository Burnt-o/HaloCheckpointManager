#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class UpdateTriggerLastCheckedUntemplated { 
public:
	virtual ~UpdateTriggerLastCheckedUntemplated() = default;
	// virtual bool isPointInsideTrigger(const SimpleMath::Vector3& worldPoint, const uintptr_t triggerData, const uint32_t triggerIndex) = 0;
};
class UpdateTriggerLastChecked : public IOptionalCheat
{
private:

	std::unique_ptr<UpdateTriggerLastCheckedUntemplated> pimpl;

public:
	UpdateTriggerLastChecked(GameState gameImpl, IDIContainer& dicon);
	~UpdateTriggerLastChecked();

	//bool isPointInsideTrigger(const SimpleMath::Vector3& worldPoint, const uintptr_t triggerData, const uint32_t triggerIndex) { return pimpl->isPointInsideTrigger(worldPoint, triggerData, triggerIndex); }

	std::string_view getName() override { return nameof(UpdateTriggerLastChecked); }

};