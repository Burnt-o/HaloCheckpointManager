#include "pch.h"
#include "Setting.h"

void Setting<bool>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_bool();
	PLOG_DEBUG << "true?1 " << valueDisplay;
	UpdateValueWithInput();
	PLOG_DEBUG << "true?2 " << value;
}

void Setting<std::string>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_string();
	UpdateValueWithInput();
}

void Setting<double>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_double();
	UpdateValueWithInput();
}
void Setting<float>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_float();
	UpdateValueWithInput();
}

void Setting<int>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_int();
	UpdateValueWithInput();
}




template<>
void Setting<bool>::flipBoolSetting()
{
	valueDisplay = !valueDisplay;
	UpdateValueWithInput();
}


