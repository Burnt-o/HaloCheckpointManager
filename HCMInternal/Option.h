#pragma once

#include <pugixml.hpp>
class SerialisableOption
{
private:
	bool mIsIncludedInClipboard;
public:
	virtual void serialise(pugi::xml_node parent) = 0;
	virtual void deserialise(pugi::xml_node input) = 0;
	virtual std::string getOptionName() = 0;
	explicit SerialisableOption() {}
};

template <typename T>
class Option : public SerialisableOption
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

	explicit Option(T defaultValue, std::function<bool(T)> inputValidator, std::string optionName)
		: isInputValid(inputValidator), value(defaultValue), valueDisplay(defaultValue), mOptionName(optionName)
	{}


	eventpp::CallbackList<void(T& newValue)> valueChangedEvent;

	void UpdateValueWithInput()
	{
		if (isInputValid(valueDisplay))
		{
			value = valueDisplay;
			valueChangedEvent(value);
		}
		else
		{
			valueDisplay = value; // reset valueDisplay back to stored value
		}
	}

	// Called by RuntimeExceptionHandler if associated code throws an exception
	void resetToDefaultValue()
	{
		value = defaultValue;
		valueDisplay = value;
		valueChangedEvent(value);
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

	// Should work for any type T
	void serialise(pugi::xml_node parent) override
	{
		auto node = parent.append_child(getShortName(getOptionName()).c_str());

		std::ostringstream out;
		out << value;

		node.text().set(out.str().c_str());

		return;
	}

	void deserialise(pugi::xml_node input) override; // Templated, see Option.cpp


};



