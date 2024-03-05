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
	PLOG_INFO << " BinarySetting<Vec2>::deserialise: " << s;
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

	valueDisplay = out;
	UpdateValueWithInput();
}

void BinarySetting<SimpleMath::Vector3>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " BinarySetting<SimpleMath::Vector3>::deserialise: " << s;
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


	valueDisplay = out;
	UpdateValueWithInput();
}

void BinarySetting<SimpleMath::Vector4>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " BinarySetting<SimpleMath::Vector4>::deserialise: " << s;
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


	valueDisplay = out;
	UpdateValueWithInput();
}






template<>
void BinarySetting<bool>::flipBoolSetting()
{
	valueDisplay = !valueDisplay;
	UpdateValueWithInput();
}


