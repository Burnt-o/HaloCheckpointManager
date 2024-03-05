#pragma once

#include "BinarySetting.h"
class ISettingsSerialiser
{
public:
	virtual void serialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions) = 0;
	virtual void deserialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions) = 0;

	~ISettingsSerialiser() = default;
};