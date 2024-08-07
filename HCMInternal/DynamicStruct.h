#pragma once





template<typename fieldEnum> 
class DynamicStruct
{


	typedef int fieldOffset;
	std::array<fieldOffset, magic_enum::enum_count<fieldEnum>()> fieldOffsetMap{};



public:
	template<typename fieldType>
	fieldType* field(fieldEnum whichField) // is there a way I could check that the fieldType matches with the enum? would probably require recursion
	{
		uintptr_t pField = currentBaseAddress + fieldOffsetMap.at(magic_enum::enum_integer(whichField));

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

	void setIndex(uintptr_t base, int i) noexcept
	{
		this->currentBaseAddress = (base + (StrideSize * i));
	}


	friend class DynamicStructFactory;
};



// example for intellisense
enum class exampleDynamicStructForIntellisense
{
	firstField, secondField
};


