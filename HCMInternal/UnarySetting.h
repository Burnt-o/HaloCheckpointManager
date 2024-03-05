#pragma once
#include "imgui.h"
#include "SerialisableSetting.h"


template <typename T>
class UnarySetting : public SerialisableSetting
{
private:
	T value;
	std::string mOptionName;

public:


	explicit UnarySetting(T defaultValue, std::string optionName)
		: value(std::move(defaultValue)), mOptionName(optionName)
	{}


	std::shared_ptr<eventpp::CallbackList<void(T& newValue)>> valueChangedEvent = std::make_shared<eventpp::CallbackList<void(T & newValue)>>();

	


	T& GetValue()
	{
		return value;
	}


	std::string getOptionName() override { return mOptionName; } // used in serialisation and user facing error messages

	// Should work for any type T that implements stringstream << 
	void serialise(pugi::xml_node parent) override
	{
		auto node = parent.append_child(getOptionName().c_str());

		std::ostringstream out;
		out << std::setprecision(8) << value;

		node.text().set(out.str().c_str());

		return;
	}

	void deserialise(pugi::xml_node input) override; // Templated, see Option.cpp

	void serialiseToClipboard()
	{
		pugi::xml_document doc;
		pugi::xml_node node = doc.append_child("");
		serialise(node);
		std::ostringstream ss;
		node.first_child().print(ss);
		ImGui::SetClipboardText(ss.str().c_str());
	}

	void deserialiseFromClipboard()
	{

		pugi::xml_document doc;
		auto parseResult = doc.load_string(ImGui::GetClipboardText()); // todo check parse result
		if (!parseResult)
		{
			throw HCMRuntimeException(std::format("Error parsing clipboard text \"{}\": {}", ImGui::GetClipboardText(), parseResult.description()));
		}

		deserialise(doc.first_child());
	}

	void flipBoolSetting() {
		PLOG_FATAL << "cannot call flipBoolSetting on non-bool setting";	throw HCMRuntimeException("cannot call flipBoolSetting on non-bool setting");
	}

};

template<>
void UnarySetting<bool>::flipBoolSetting(); // only defined for bool


