#include "pch.h"
#include "BinarySetting.h"
#include "SettingsEnums.h"
#include <concepts>

void BinarySetting<bool>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_bool();
	PLOG_DEBUG << "deserialised bool value: " << (valueDisplay ? "True" : "False");
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

// for enums
template<typename T>
void BinarySetting<T>::deserialise(pugi::xml_node input)
{
	static_assert(std::is_enum_v<T>);
	valueDisplay = (T)input.text().as_int();
	UpdateValueWithInput();
}
using namespace SettingsEnums;
template void BinarySetting<FreeCameraObjectTrackEnum>::deserialise(pugi::xml_node input);
template void BinarySetting<FreeCameraObjectTrackEnumPlusAbsolute>::deserialise(pugi::xml_node input);
template void BinarySetting<FreeCameraInterpolationTypesEnum>::deserialise(pugi::xml_node input);
template void BinarySetting<TriggerRenderStyle>::deserialise(pugi::xml_node input);
template void BinarySetting<TriggerInteriorStyle>::deserialise(pugi::xml_node input);
template void BinarySetting<TriggerLabelStyle>::deserialise(pugi::xml_node input);
template void BinarySetting<ScreenAnchorEnum>::deserialise(pugi::xml_node input);
template void BinarySetting<SoftCeilingRenderTypes>::deserialise(pugi::xml_node input);
template void BinarySetting<SoftCeilingRenderDirection>::deserialise(pugi::xml_node input);




template<>
void BinarySetting<bool>::flipBoolSetting()
{
	valueDisplay = !valueDisplay;
	UpdateValueWithInput();
}


