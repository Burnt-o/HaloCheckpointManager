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

void Setting<unsigned int>::deserialise(pugi::xml_node input)
{
	valueDisplay = input.text().as_uint();
	UpdateValueWithInput();
}

void Setting<SimpleMath::Vector2>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " Setting<Vec2>::deserialise: " << s;
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

void Setting<SimpleMath::Vector3>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " Setting<SimpleMath::Vector3>::deserialise: " << s;
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

void Setting<SimpleMath::Vector4>::deserialise(pugi::xml_node input)
{
	std::string s = input.first_child().text().as_string();
	PLOG_INFO << " Setting<SimpleMath::Vector4>::deserialise: " << s;
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
void Setting<bool>::flipBoolSetting()
{
	valueDisplay = !valueDisplay;
	UpdateValueWithInput();
}


