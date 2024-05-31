#pragma once
#include "pch.h"
#include "TriggerModel.h"
#include <bitset>
struct TriggerData 
{
	std::string name;
	uint32_t tickLastChecked = 0;
	TriggerModel model;


private:
	friend class GetTriggerDataUntemplated;
	explicit TriggerData(std::string triggerName, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up) : name(triggerName), model(triggerName, position, extents, forward, up) {}
};