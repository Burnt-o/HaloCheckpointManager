#include "pch.h"
#include "GetObjectAddress.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "DynamicStructFactory.h"
#include "ModuleHook.h"


typedef int32_t enumBitmask;

extern "C" typedef uintptr_t __fastcall getObjectAddressFunc(Datum datum, enumBitmask expectedObjectTypeBitMask);

// Just calls the games inbuilt function for resolving object addreses from datum
class GetObjectAddress::GetObjectAddressImpl
{
	GameState mGame;


	// data
	std::shared_ptr<MultilevelPointer> getObjectAddressFunction;

	template <typename TemplatedObjectType>
	enumBitmask expectedObjectTypeToBitmask(std::set <TemplatedObjectType> expectedObjectTypes)
	{
		// the games function actually takes a bit mask, so you can match against multiple expected object types.
		if (expectedObjectTypes.empty()) return -1;
		
		enumBitmask out;
		for (auto& expectedObjectType : expectedObjectTypes)
		{
			if ((int32_t)expectedObjectType == -1) return -1; // null means catch all
			out = out | ((int32_t)1 << (int32_t)expectedObjectType); // set bit at nth position where n = expectedObjectType
		}
		
		return out;
	}

public:

	template <typename TemplatedObjectType>
	uintptr_t getObjectAddress(Datum datum, std::set < TemplatedObjectType> expectedObjectTypes)
	{
		LOG_ONCE(PLOG_DEBUG << "getObjectAddress");

		// resolve pointer to games getObjectAddressFunction
		uintptr_t p_getObjectAddressFunc;
		if (!getObjectAddressFunction->resolve(&p_getObjectAddressFunc)) throw HCMRuntimeException(std::format("could not resolve games getObjectAddressFunc: {}", MultilevelPointer::GetLastError()));

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "p_getObjectAddressFunc: " << pget, pget = p_getObjectAddressFunc);

		LOG_ONCE(PLOG_DEBUG << "calling games getObjectAddressFunc");
		
		auto result = ((getObjectAddressFunc*)p_getObjectAddressFunc)(datum, expectedObjectTypeToBitmask<TemplatedObjectType>(expectedObjectTypes));
		if (!result) throw HCMRuntimeException("null object address");
		return result;
	}

	GetObjectAddressImpl(GameState game, IDIContainer& dicon) 
		: mGame(game)
	{
		getObjectAddressFunction = dicon.Resolve<PointerManager>()->getData<std::shared_ptr<MultilevelPointer>>(nameof(getObjectAddressFunction), game);
	}
};

GetObjectAddress::GetObjectAddress(GameState game, IDIContainer& dicon) : pimpl(std::make_unique<GetObjectAddressImpl>(game, dicon)) {}
GetObjectAddress::~GetObjectAddress() = default;


uintptr_t GetObjectAddress::getObjectAddress(Datum d, std::set < CommonObjectType> o) { return pimpl->getObjectAddress<CommonObjectType>(d, o); }
template <typename TemplatedObjectType>
uintptr_t GetObjectAddress::getObjectAddress(Datum d, std::set < TemplatedObjectType> o) { return pimpl->getObjectAddress<TemplatedObjectType>(d, o); }


template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo1ObjectType>(Datum d, std::set<Halo1ObjectType> o) { return pimpl->getObjectAddress< Halo1ObjectType>(d, o); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo2ObjectType>(Datum d, std::set < Halo2ObjectType> o) { return pimpl->getObjectAddress< Halo2ObjectType>(d, o); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo3ObjectType>(Datum d, std::set < Halo3ObjectType> o) { return pimpl->getObjectAddress< Halo3ObjectType>(d, o); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo3ODSTObjectType>(Datum d, std::set < Halo3ODSTObjectType> o) { return pimpl->getObjectAddress< Halo3ODSTObjectType>(d, o); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<HaloReachObjectType>(Datum d, std::set < HaloReachObjectType> o) { return pimpl->getObjectAddress< HaloReachObjectType>(d, o); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo4ObjectType>(Datum d, std::set < Halo4ObjectType> o) { return pimpl->getObjectAddress< Halo4ObjectType>(d, o); }


