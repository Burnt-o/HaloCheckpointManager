#include "pch.h"
#include "SettingsSerialiser.h"
#include "SettingsStateAndEvents.h"
#include "MessagesGUI.h"
#include <pugixml.hpp>


// need to get some xml code up in here

constexpr std::string_view configFileName = "HCMInternalConfig.xml";

void throwOnDuplicateName(pugi::xml_document& doc)
{
	// recursive interface to test if all tag names with the same parent are unique 
	struct simple_walker : pugi::xml_tree_walker
	{
;
		virtual bool for_each(pugi::xml_node& node)
		{
			if (node.name() == "") return true;

			std::set<std::string> checkForDuplicateSymbols;
			for (pugi::xml_node child : node.children())
			{	
				if (child.type() == pugi::node_null) continue;
				if (checkForDuplicateSymbols.emplace(child.name()).second)
				{
					continue;
				}
				else
				{
					throw HCMSerialisationException(std::format("Non-unique setting name \"{}\"! Burnt made an oopsie", child.name()));
				}
			}
		}
	}walker;
	doc.traverse(walker);
}




void SettingsSerialiser::serialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions)
{
	std::string filePath = mDirPath + configFileName.data();
	PLOG_DEBUG << "Saving settings to filepath: " << filePath;
	try
	{
		pugi::xml_document doc;
		// options
		auto optionArray = doc.append_child(nameof(Setting));

		for (auto& option : allSerialisableOptions)
		{
			PLOG_DEBUG << "Serialising setting: " << option->getOptionName();
			option->serialise(optionArray);
		}


		throwOnDuplicateName(doc); 

		if (!doc.save_file(filePath.c_str()))
		{
			PLOG_ERROR << "Error saving config to " << filePath;
		}
		else
		{
			PLOG_DEBUG << "Successfully saved settings to filepath: " << filePath;
		}
	}
	catch (HCMSerialisationException& ex)
	{
		runtimeExceptions->handleMessage(ex);
	}
}

void SettingsSerialiser::deserialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions)
{
	std::string filePath = mDirPath + configFileName.data();
	PLOG_DEBUG << "filePath to find setting serialisation file: " << filePath;
	pugi::xml_document doc;
			
	pugi::xml_parse_result result = doc.load_file(filePath.c_str());
	PLOG_DEBUG << "setting file parse result: " << (result.operator bool() ? "true" : "false");
	if (result)
	{
		try
		{
			auto optionArray = doc.child(nameof(Setting));
			if (optionArray.type() == pugi::node_null) 
				throw HCMSerialisationException("Could not find OptionArray node");
			for (auto& option : allSerialisableOptions)
			{
				PLOG_VERBOSE << "Deserialising setting: " << option->getOptionName();
				auto optionXML = optionArray.child(option->getOptionName().c_str());
				if (optionXML.type() == pugi::node_null)
				{
					messagesGUI->addMessage(std::format("Couldn't find saved value of option: {}, using default value.", option->getOptionName()));
					continue;
				}

				PLOG_VERBOSE << "with value: [" << optionXML.text() << "]";
				option->deserialise(optionXML);
			}
		}
		catch (HCMSerialisationException& ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
	else
	{
		PLOG_ERROR << "Error parsing settings file";
		std::string resultString = result.description();
		if (resultString == "File was not found")
		{
			messagesGUI->addMessage("Config file not found, loading default settings.");
		}
		else
		{
			std::string err = std::format("Error parsing file at {}\nError description: {}\nError offset: {}", filePath, resultString, result.offset);
			HCMSerialisationException ex(err);
			runtimeExceptions->handleMessage(ex);
		}


	}
}




