#pragma once
#include "Hotkey.h"
#include <pugixml.hpp>
#include "WindowsUtilities.h" // could move to cpp file
class SerialisableSetting
{
private:
	bool mIsIncludedInClipboard;
public:
	virtual void serialise(pugi::xml_node parent) = 0;
	virtual void deserialise(pugi::xml_node input) = 0;
	virtual std::string getOptionName() = 0;
	explicit SerialisableSetting() {}
};

template <typename T>
class Setting : public SerialisableSetting
{
private:
	T value;
	T valueDisplay; // copy of value for user input
	T defaultValue; // used in case we need to reset
	std::function<bool(T)> isInputValid;
	std::string mOptionName;


public:

	//// bool options must be named
	//explicit Option(bool defaultValue, std::function<bool(bool)> inputValidator, std::string optionName)
	//	: isInputValid(inputValidator), value(defaultValue), valueDisplay(defaultValue), mOptionName(optionName)
	//{}

	explicit Setting(T defaultValue, std::function<bool(T)> inputValidator, std::string optionName)
		: isInputValid(inputValidator), value(defaultValue), valueDisplay(defaultValue), mOptionName(optionName)
	{}


	std::shared_ptr<eventpp::CallbackList<void(T& newValue)>> valueChangedEvent = std::make_shared<eventpp::CallbackList<void(T & newValue)>>();

	void UpdateValueWithInput()
	{
		if (isInputValid(valueDisplay))
		{
			value = valueDisplay;
			PLOG_VERBOSE << "Firing value changed event for " << mOptionName;
			valueChangedEvent.get()->operator()(value);
			//valueChangedEvent->operator ()(value);
		}
		else
		{
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
		auto node = parent.append_child(getShortName(getOptionName()).c_str());

		std::ostringstream out;
		out << value;

		node.text().set(out.str().c_str());

		return;
	}

	void deserialise(pugi::xml_node input) override; // Templated, see Option.cpp


	void flipBoolSetting() {
		PLOG_FATAL << "cannot call flipBoolSetting on non-bool setting";	throw HCMRuntimeException("cannot call flipBoolSetting on non-bool setting");
	}

};

template<>
void Setting<bool>::flipBoolSetting(); // only defined for bool


