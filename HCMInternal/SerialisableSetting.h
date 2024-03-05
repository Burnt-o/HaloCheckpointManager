#pragma once
#include <pugixml.hpp>

class SerialisableSetting
{
private:
	bool mIsIncludedInClipboard;
public:
	virtual void serialise(pugi::xml_node parent) = 0;
	virtual void deserialise(pugi::xml_node input) = 0;
	virtual std::string getOptionName() = 0;
	explicit SerialisableSetting() {}
};