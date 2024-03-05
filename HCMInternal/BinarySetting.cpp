#include "pch.h"
#include "BinarySetting.h"


void BinarySetting<bool>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_bool();
	PLOG_DEBUG << "Deserialised bool value: " << (valueDisplay ? "True" : "False");
	UpdateValueWithInput();
	if (value != valueDisplay) PLOG_ERROR << "Value didn't match after UpdateValueWithInput()!";
}

void BinarySetting<std::string>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_string();
	UpdateValueWithInput();
}

void BinarySetting<double>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_double();
	UpdateValueWithInput();
}
void BinarySetting<float>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_float();
	UpdateValueWithInput();
}

void BinarySetting<int>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_int();
	UpdateValueWithInput();
}

void BinarySetting<unsigned int>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_uint();
	UpdateValueWithInput();
}

void BinarySetting<SimpleMath::Vector2>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " BinarySetting<SimpleMath::Vector2>::deserialise: " << s;

	valueDisplay = vec2FromString(s);
	UpdateValueWithInput();
}

void BinarySetting<SimpleMath::Vector3>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " BinarySetting<SimpleMath::Vector3>::deserialise: " << s;

	valueDisplay = vec3FromString(s);
	UpdateValueWithInput();
}

void BinarySetting<SimpleMath::Vector4>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " BinarySetting<SimpleMath::Vector4>::deserialise: " << s;

	valueDisplay = vec4FromString(s);
	UpdateValueWithInput();
}






template<>
void BinarySetting<bool>::flipBoolSetting()
{
	valueDisplay = !valueDisplay;
	UpdateValueWithInput();
}


