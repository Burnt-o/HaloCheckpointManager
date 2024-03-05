#pragma once
#include "SerialisableSetting.h"
#include "imgui.h"

template <typename T>
class BinarySetting : public SerialisableSetting
{
private:
	T value;
	T valueDisplay; // copy of value for user input
	T defaultValue; // used in case we need to reset
	std::function<bool(T)> isInputValid;
	std::string mOptionName;
	std::mutex updateValueWithInputMutex{};

public:

	//// bool options must be named
	//explicit Option(bool defaultValue, std::function<bool(bool)> inputValidator, std::string optionName)
	//	: isInputValid(inputValidator), value(defaultValue), valueDisplay(defaultValue), mOptionName(optionName)
	//{}

	explicit BinarySetting(T defaultValue, std::function<bool(T)> inputValidator, std::string optionName)
		: isInputValid(inputValidator), value(defaultValue), valueDisplay(defaultValue), mOptionName(optionName)
	{}


	std::shared_ptr<eventpp::CallbackList<void(T& newValue)>> valueChangedEvent = std::make_shared<eventpp::CallbackList<void(T & newValue)>>();

	void UpdateValueWithInput()
	{
		std::lock_guard<std::mutex> lock(updateValueWithInputMutex);

		if (isInputValid(valueDisplay))
		{
			value = valueDisplay;
			PLOG_VERBOSE << "Firing value changed event for " << mOptionName;
			valueChangedEvent.get()->operator()(value);
			//valueChangedEvent->operator ()(value);
		}
		else
		{
			PLOG_ERROR << "Input failed validation for setting: " << mOptionName << std::endl << "old setting: " << value << std::endl << "bad new setting: " << valueDisplay;
			valueDisplay = value; // reset valueDisplay back to stored value
		}
	}

	
	void resetToDefaultValue()
	{
		value = defaultValue;
		valueDisplay = value;
		valueChangedEvent->operator ()(value);
	}

	T& GetValue()
	{
		return value;
	}

	T& GetValueDisplay()
	{
		return valueDisplay;
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
void BinarySetting<bool>::flipBoolSetting(); // only defined for bool


