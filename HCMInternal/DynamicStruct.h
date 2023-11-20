#pragma once

// this might be more of a complicated macro than a class?
// actually maybe more of an interface that defines what clients can DO with any given struct ..
// ah bout what about accessing the fields? by.. enum? 
// template by enum class?




template<typename fieldEnum> 
class DynamicStruct
{


	typedef int fieldOffset;
	std::unordered_map<fieldEnum, fieldOffset> fieldOffsetMap{};



public:
	template<typename fieldType>
	fieldType* field(fieldEnum whichField) // is there a way I could check that the fieldType matches with the enum? would probably require recursion
	{
		if (!fieldOffsetMap.contains(whichField)) throw HCMRuntimeException("?!");
		uintptr_t pField = currentBaseAddress + fieldOffsetMap.at(whichField);
		if (IsBadReadPtr((void*)pField, sizeof(fieldType))) throw HCMRuntimeException(std::format("Bad read ptr at field: {}, address: 0x{:X}", magic_enum::enum_name(whichField), pField));

		return (fieldType*)pField;
	}

	uintptr_t currentBaseAddress;

	friend class DynamicStructFactory;
};
template<typename fieldEnum>
class StrideableDynamicStruct : public DynamicStruct<fieldEnum>
{
public:
	int StrideSize; // I intended to keep this private but "friend class DynamicStructFactory" isn't working how I thought it would
public: 
	void operator++ () noexcept
	{
		this->currentBaseAddress += StrideSize;
	}

	void setIndex(int i) noexcept
	{
		this->currentBaseAddress += (StrideSize * i);
	}


	friend class DynamicStructFactory;
};



// example for intellisense
enum class exampleDynamicStructForIntellisense
{
	firstField, secondField
};


