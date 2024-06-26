#include "pch.h"
#include "GetDebugString.h"
#include "DynamicStructFactory.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"

// just doing the h2 impl for now



class GetDebugString::GetDebugStringImpl
{
public:

	enum class stringIDDataFields { stringIndex, stringLength };
	std::shared_ptr<DynamicStruct<stringIDDataFields>> stringIDDataStruct;


	std::shared_ptr<MultilevelPointer> debugStringTableAddress;
	std::shared_ptr<MultilevelPointer> debugStringMetaTableAddress;
	// TODO: caching. meh strings are expensive anyway


	GetDebugStringImpl(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		stringIDDataStruct = DynamicStructFactory::make<stringIDDataFields>(ptr, game);
		debugStringTableAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(debugStringTableAddress), game);
		debugStringMetaTableAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(debugStringMetaTableAddress), game);
	}


	std::expected<std::string, HCMRuntimeException> getDebugString(uintptr_t pStringID)
	{ 
		uintptr_t debugStringTableAddressResolved;
		if (!debugStringTableAddress->resolve(&debugStringTableAddressResolved)) 
			return std::unexpected(HCMRuntimeException(std::format("Failed to resolve debugStringTableAddress, {}", MultilevelPointer::GetLastError())));
		uintptr_t debugStringMetaTableAddressResolved;
		if (!debugStringMetaTableAddress->resolve(&debugStringMetaTableAddressResolved))
			return std::unexpected(HCMRuntimeException(std::format("Failed to resolve debugStringMetaTableAddress, {}", MultilevelPointer::GetLastError())));
	
		stringIDDataStruct->currentBaseAddress = pStringID;

		auto* pStringLength = stringIDDataStruct->field<uint8_t>(stringIDDataFields::stringLength);
		if (IsBadReadPtr(pStringLength, sizeof(uint8_t))) return std::unexpected(HCMRuntimeException(std::format("Bad read address for pStringLength at {}", (uintptr_t)pStringLength)));

		auto* pStringIndex = stringIDDataStruct->field<uint16_t>(stringIDDataFields::stringIndex);
		if (IsBadReadPtr(pStringIndex, sizeof(uint16_t))) return std::unexpected(HCMRuntimeException(std::format("Bad read address for pStringIndex at {}", (uintptr_t)pStringIndex)));

		// could dynstruct the meta info.. will have to see how h3+ handles it
		auto pNameOffset = debugStringMetaTableAddressResolved + (*pStringIndex * sizeof(uint32_t));
		if (IsBadReadPtr((void*)pNameOffset, sizeof(uint32_t))) return  std::unexpected(HCMRuntimeException(std::format("Bad read address for pNameOffset at {}", (uintptr_t)pNameOffset)));

		auto pName = debugStringTableAddressResolved + *(uint32_t*)pNameOffset;

		std::string name((char*)pName);
		if (name.length() != *pStringLength) return std::unexpected(HCMRuntimeException(std::format("Debug string length mismatch: expected 0x{:X}, observed 0x{:X}", *pStringLength, name.length())));

		return name; // ya ya making a copy is slow
	}
};

GetDebugString::GetDebugString(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique< GetDebugStringImpl>(game, dicon);
}

GetDebugString::~GetDebugString() = default;

std::expected<std::string, HCMRuntimeException> GetDebugString::getDebugString(uintptr_t pStringID) { return pimpl->getDebugString(pStringID); }