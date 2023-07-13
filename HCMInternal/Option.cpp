#include "pch.h"
#include "Option.h"

void Option<bool>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_bool();
	PLOG_DEBUG << "true?1 " << valueDisplay;
	UpdateValueWithInput();
	PLOG_DEBUG << "true?2 " << value;
}

void Option<std::string>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_string();
	UpdateValueWithInput();
}

void Option<double>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_double();
	UpdateValueWithInput();
}
void Option<float>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_float();
	UpdateValueWithInput();
}

void Option<int>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_int();
	UpdateValueWithInput();
}

