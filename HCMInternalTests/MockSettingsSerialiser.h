#pragma once
#include "ISettingsSerialiser.h"

class MockSettingsSerialiser : public ISettingsSerialiser
{
public:
	virtual void serialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions) override {}
	virtual void deserialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions) override {}
};