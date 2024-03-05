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
	PLOG_INFO << " UnarySetting<Vec2>::deserialise: " << s;
	std::stringstream ss(s);

	SimpleMath::Vector2 out(0.f, 0.f); // default value if serialisation fails
	std::string xString, yString;


	std::getline(ss, xString, ',');
	std::getline(ss, yString);


	// remove whitespace
	std::erase_if(xString, [](const char& c) { return std::isspace(c); });
	std::erase_if(yString, [](const char& c) { return std::isspace(c); });


	// strtof does not throw, just returns 0.f if conversion failed. That's fine.
	out.x = std::strtof(xString.c_str(), nullptr);
	out.y = std::strtof(yString.c_str(), nullptr);

	value = out;
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<SimpleMath::Vector3>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " UnarySetting<SimpleMath::Vector3>::deserialise: " << s;
	std::stringstream ss(s);

	SimpleMath::Vector3 out(0.f, 0.f, 0.f); // default value if serialisation fails
	std::string xString, yString, zString;


	std::getline(ss, xString, ',');
	std::getline(ss, yString, ',');
	std::getline(ss, zString);

	// remove whitespace
	std::erase_if(xString, [](const char& c) { return std::isspace(c); });
	std::erase_if(yString, [](const char& c) { return std::isspace(c); });
	std::erase_if(zString, [](const char& c) { return std::isspace(c); });

	// strtof does not throw, just returns 0.f if conversion failed. That's fine.
	out.x = std::strtof(xString.c_str(), nullptr);
	out.y = std::strtof(yString.c_str(), nullptr);
	out.z = std::strtof(zString.c_str(), nullptr);


	value = out;
	valueChangedEvent.get()->operator()(value);
}

void UnarySetting<SimpleMath::Vector4>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " UnarySetting<SimpleMath::Vector4>::deserialise: " << s;
	std::stringstream ss(s);

	SimpleMath::Vector4 out(0.f, 0.f, 0.f, 0.f); // default value if serialisation fails
	std::string xString, yString, zString, wString;


	std::getline(ss, xString, ',');
	std::getline(ss, yString, ',');
	std::getline(ss, zString, ',');
	std::getline(ss, wString);

	// remove whitespace
	std::erase_if(xString, [](const char& c) { return std::isspace(c); });
	std::erase_if(yString, [](const char& c) { return std::isspace(c); });
	std::erase_if(zString, [](const char& c) { return std::isspace(c); });
	std::erase_if(wString, [](const char& c) { return std::isspace(c); });

	// strtof does not throw, just returns 0.f if conversion failed. That's fine.
	out.x = std::strtof(xString.c_str(), nullptr);
	out.y = std::strtof(yString.c_str(), nullptr);
	out.z = std::strtof(zString.c_str(), nullptr);
	out.w = std::strtof(wString.c_str(), nullptr);


	value = out;
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


