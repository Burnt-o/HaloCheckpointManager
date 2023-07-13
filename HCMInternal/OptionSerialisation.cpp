#include "pch.h"
#include "OptionSerialisation.h"
#include "OptionsState.h"
#include "MessagesGUI.h"
#include "HCMDirPath.h"
namespace OptionSerialisation
{
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
						throw HCMSerialisationException(std::format("Non-unique acronym \"{}\"! Burnt made an oopsie", child.name()));
					}
				}
			}
		}walker;
		doc.traverse(walker);
	}





	pugi::xml_document serialiseAllOptions()
	{
		pugi::xml_document doc;
		// options
		auto optionArray = doc.append_child(acronymOf(Option));

		for (auto option : OptionsState::allSerialisableOptions)
		{
			PLOG_DEBUG << "serialising: " << option->getOptionName();
			option->serialise(optionArray);
		}


		throwOnDuplicateName(doc); // rules are allowed to be duplicate names since you can have multiple of the same type

		return doc;

	}



	void deserialiseAllOptions(pugi::xml_document& doc, bool clipboardOnly)
	{

		// options
		auto optionArray = doc.child(acronymOf(Option));
		if (optionArray.type() == pugi::node_null) throw HCMSerialisationException("Could not find OptionArray node");
		for (auto option : OptionsState::allSerialisableOptions)
		{
			auto optionXML = optionArray.child(getShortName(option->getOptionName()).c_str());
			if (optionXML.type() == pugi::node_null) throw HCMSerialisationException(std::format("Could not find Option node {}", option->getOptionName()));
			option->deserialise(optionXML);
		}

	}




	void serialiseToFile()
	{
		std::string filePath (HCMDirPath::GetHCMDirPath());
		filePath += configFileName;

		try
		{
			auto xml = serialiseAllOptions();
			if (!xml.save_file(filePath.c_str()))
			{
				PLOG_ERROR << "Error saving config to " << filePath;
			}
		}
		catch (HCMSerialisationException& ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
		}
	}
	void deserialiseFromFile()
	{
		std::string filePath(HCMDirPath::GetHCMDirPath());
		filePath += configFileName;
		
		pugi::xml_document doc;
			
		pugi::xml_parse_result result = doc.load_file(filePath.c_str());

		if (result)
		{
			try
			{
				deserialiseAllOptions(doc, false);
			}
			catch (HCMSerialisationException& ex)
			{
				RuntimeExceptionHandler::handleMessage(ex);
			}
		}
		else
		{
			std::string resultString = result.description();
			if (resultString == "File was not found")
			{
				MessagesGUI::addMessage("Config file not found, loading default settings.");
			}
			else
			{
				std::string err = std::format("Error parsing file at {}\nError description: {}\nError offset: {}", filePath, resultString, result.offset);
				HCMSerialisationException ex(err);
				RuntimeExceptionHandler::handleMessage(ex);
			}


		}
	}





}