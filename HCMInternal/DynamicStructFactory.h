#pragma once
#include "DynamicStruct.h"
#include "PointerDataStore.h"


typedef std::map<std::string, int> DynStructOffsetInfo; 

// intellisense fucking HATES this thing lmao

// Creates DynamicStructs, resolving the necessary data from PointerDataStore using the name of the provided fieldEnum template
class DynamicStructFactory
{
public:
	template<typename fieldEnum>
	static std::shared_ptr<DynamicStruct<fieldEnum>> make(std::shared_ptr<PointerDataStore> ptr, std::optional<GameState> game = std::nullopt)
	{
		// make an uninitialized dynamic struct of the type associated with the fieldEnum
		std::shared_ptr<DynamicStruct<fieldEnum>> outDynStruct = std::make_shared<DynamicStruct<fieldEnum>>();

		// fiendEnum to string. for looking up data from pointerDataStore (and logging)
		std::string enumClassName = magic_enum::enum_type_name<fieldEnum>().data();

		PLOG_DEBUG << "Constructing DynamicStruct for fieldEnum: " << enumClassName;

		// get the DynStructOffsetInfo for this fieldEnum's name. 
		// a DynStructOffsetInfo maps each field name to the int offset.
		auto stringFieldOffsetMap = ptr->getData<std::shared_ptr<DynStructOffsetInfo>>(magic_enum::enum_type_name<fieldEnum>().data(), game);

		// for each field value in the field enum
		magic_enum::enum_for_each<fieldEnum>([fom = &outDynStruct->fieldOffsetMap, sfom = stringFieldOffsetMap.get(), ecn = enumClassName](auto field)
			{
				// convert field value name to a string
				constexpr fieldEnum this_fieldEnum = field;
				std::string fieldName = magic_enum::enum_name(this_fieldEnum).data();

				// check if the DynStructOffsetInfo contains the field value name
				if (!sfom->contains(fieldName)) throw HCMInitException(std::format("stringFieldOffsetMap for {} was missing field: {}", ecn, fieldName));

				// get the int offset from the DynStructOffsetInfo and assign it to our DynamicStruct 
				fom->at(magic_enum::enum_integer(this_fieldEnum)) = sfom->at(fieldName);
			});
		return outDynStruct;
	}

	template<typename fieldEnum>
	static std::shared_ptr<StrideableDynamicStruct<fieldEnum>> makeStrideable(std::shared_ptr<PointerDataStore> ptr, std::optional<GameState> game = std::nullopt)
	{
		// make an uninitialized dynamic struct of the type associated with the fieldEnum
		std::shared_ptr<StrideableDynamicStruct<fieldEnum>> outDynStruct = std::make_shared<StrideableDynamicStruct<fieldEnum>>();

		// fiendEnum to string. for looking up data from pointerDataStore (and logging)
		std::string enumClassName = magic_enum::enum_type_name<fieldEnum>().data();

		PLOG_DEBUG << "Constructing DynamicStruct for fieldEnum: " << enumClassName;

		// get the DynStructOffsetInfo for this fieldEnum's name. 
		// a DynStructOffsetInfo maps each field name to the int offset.
		auto stringFieldOffsetMap = ptr->getData<std::shared_ptr<DynStructOffsetInfo>>(magic_enum::enum_type_name<fieldEnum>().data(), game);

		// check the DynStructOffsetInfo for an entry named "StrideSize". This info is mandatory for StrideableDynamicStructs
		if (!stringFieldOffsetMap->contains("StrideSize")) throw HCMInitException("StrideableDynamicStruct missing entry for STrideSize");

		// assign StrideableDynamicStruct's stride size
		outDynStruct->StrideSize = stringFieldOffsetMap->at("StrideSize");
		// for each field value in the field enum
		magic_enum::enum_for_each<fieldEnum>([fom = &outDynStruct->fieldOffsetMap, sfom = stringFieldOffsetMap.get(), ecn = enumClassName](auto field)
			{
				// convert field value name to a string
				constexpr fieldEnum this_fieldEnum = field;
				std::string fieldName = magic_enum::enum_name(this_fieldEnum).data();

				// check if the DynStructOffsetInfo contains the field value name
				if (!sfom->contains(fieldName)) throw HCMInitException(std::format("stringFieldOffsetMap for {} was missing field: {}", ecn, fieldName));
				
				// get the int offset from the DynStructOffsetInfo and assign it to our DynamicStruct 
				fom->at(magic_enum::enum_integer(this_fieldEnum)) = sfom->at(fieldName);
			});
		return outDynStruct;
	}


};

