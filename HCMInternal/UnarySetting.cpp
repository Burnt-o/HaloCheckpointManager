#include "pch.h"
#include "UnarySetting.h"
#include "WaypointList.h"

void UnarySetting<bool>::deserialise(pugi::xml_node input)
{
	value = input.text().as_bool();
	PLOG_DEBUG << "Deserialised bool value: " << (value ? "True" : "False");
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<std::string>::deserialise(pugi::xml_node input)
{
	value = input.text().as_string();
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<double>::deserialise(pugi::xml_node input)
{
	value = input.text().as_double();
	valueChangedEvent.get()->operator()(value);
}
void UnarySetting<float>::deserialise(pugi::xml_node input)
{
	value = input.text().as_float();
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<int>::deserialise(pugi::xml_node input)
{
	value = input.text().as_int();
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<unsigned int>::deserialise(pugi::xml_node input)
{
	value = input.text().as_uint();
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<SimpleMath::Vector2>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " UnarySetting<SimpleMath::Vector2>::deserialise: " << s;

	value = vec2FromString(s);
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<SimpleMath::Vector3>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " UnarySetting<SimpleMath::Vector3>::deserialise: " << s;

	value = vec3FromString(s);
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<SimpleMath::Vector4>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " UnarySetting<SimpleMath::Vector4>::deserialise: " << s;

	value = vec4FromString(s);
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<WaypointList>::deserialise(pugi::xml_node input)
{
	WaypointList in(input);
	value.list = in.list;
	valueChangedEvent.get()->operator()(value);
}




template<>
void UnarySetting<bool>::flipBoolSetting()
{
	value = !value;
	valueChangedEvent.get()->operator()(value);
}


