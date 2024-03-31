#pragma once
#include "pch.h"
#include "TriggerModel.h"
#include <bitset>
struct TriggerData 
{
	std::string name;
	uint32_t tickLastChecked = 0;
	TriggerModel model;

	explicit TriggerData(std::string triggerName, TriggerModel triggerModel) : name(triggerName), model(triggerModel) {}
};