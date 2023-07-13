#pragma once
#include <pugixml.hpp>
namespace OptionSerialisation
{

	extern pugi::xml_document serialiseAllOptions();
	extern void deserialiseAllOptions(pugi::xml_document& doc);

	extern void serialiseToFile();
	extern void deserialiseFromFile();

}



