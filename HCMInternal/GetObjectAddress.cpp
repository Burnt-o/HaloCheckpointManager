#include "pch.h"
#include "GetObjectAddress.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MultilevelPointer.h"
#include "IMCCStateHook.h"


template <typename TemplatedObjectTypeOrCommon>
TemplatedObjectTypeOrCommon getObjectType(uintptr_t objectTypeByte)
{
	uint8_t objectTypeValue = *(uint8_t*)objectTypeByte;

	auto objectTypeEnum = magic_enum::enum_cast<TemplatedObjectTypeOrCommon>(objectTypeValue).value_or(TemplatedObjectTypeOrCommon::FailedTypeConversion);

	LOG_ONCE_CAPTURE(PLOG_DEBUG << "return object type: " << tEnum << " of metaType " << typeid(TemplatedObjectTypeOrCommon).name(), tEnum = magic_enum::enum_name(objectTypeEnum));
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "(as int: " << asInt << ")", asInt = (int)objectTypeValue);
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "(as int2: " << asInt << ")", asInt = (int)objectTypeEnum);

	LOG_ONCE_CAPTURE(PLOG_DEBUG << "mismatch? " << (mismatchDetected ? "MISMATCH DETECTED" : "we cool"), mismatchDetected = ((int)objectTypeValue != (int)objectTypeEnum));

	return objectTypeEnum;
}




template <typename TemplatedObjectType>
class GetObjectAddressDataTable : public IGetObjectAddressImpl
{
private:
	GameState mGame;

	//data
	std::shared_ptr<MultilevelPointer> objectMetaDataTable;
	std::shared_ptr<MultilevelPointer> objectDataTable;
	uintptr_t objectMetaDataTable_cached = 0;
	uintptr_t objectDataTable_cached = 0;

	int64_t objectHeaderStride;
	int64_t objectTypeOffset;
	int64_t objectOffsetOffset;

	//callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// event to fire on mcc state change (just clear the caches)
	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing getObjectAddress caches";
		objectMetaDataTable_cached = 0;
		objectDataTable_cached = 0;
	}


	


public:
	GetObjectAddressDataTable(GameState game, IDIContainer& dicon)
		: mGame(game),
			MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); })
		{
			auto ptr = dicon.Resolve<PointerManager>().lock();
			objectMetaDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectMetaDataTable), game);

			objectHeaderStride = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderStride), game).get();
			objectTypeOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectTypeOffset), game).get();
			objectOffsetOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectOffsetOffset), game).get();
			objectDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectDataTable), game);

		}



	template <typename TemplatedOrCommonObjectType>
	uintptr_t getObjectAddress(Datum entityDatum, TemplatedOrCommonObjectType* outObjectType)
	{
		if (objectMetaDataTable_cached == 0 || objectDataTable_cached == 0)
		{
			// resolve cache
			PLOG_INFO << "resolving getObjectAddress caches";
			if (!objectMetaDataTable->resolve(&objectMetaDataTable_cached)) throw HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError()));
			if (!objectDataTable->resolve(&objectDataTable_cached)) throw HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError()));

		}
		auto ourObjectHeader = objectMetaDataTable_cached + (objectHeaderStride * entityDatum.index);
		if (outObjectType)
		{
			auto tempOutObjectType = getObjectType<TemplatedOrCommonObjectType>(ourObjectHeader + objectTypeOffset);
			PLOG_DEBUG << "as int: " << (int)tempOutObjectType;
			*outObjectType = tempOutObjectType;
		}

		auto ourObjectOffset = *(DWORD*)(ourObjectHeader + objectOffsetOffset);
		auto ourObjectAddress = objectDataTable_cached + ourObjectOffset;

		if (IsBadReadPtr((void*)ourObjectAddress, 4)) throw HCMRuntimeException(std::format("getObjectAddress: Bad read at {:X}", ourObjectAddress));
		return ourObjectAddress;
	}


	virtual uintptr_t getObjectAddressCommon(Datum entityDatum, CommonObjectType* outObjectType) override
	{
		return getObjectAddress<CommonObjectType>(entityDatum, outObjectType);
	}

	virtual uintptr_t getObjectAddressTemplated(Datum entityDatum, TemplatedObjectType* outObjectType) override
	{
		return getObjectAddress<TemplatedObjectType>(entityDatum, outObjectType);
	}




		
};


template <typename TemplatedObjectType>
class GetObjectAddressDirectPointer : public IGetObjectAddressImpl
{
private:
	GameState mGame;

	//data
	std::shared_ptr<MultilevelPointer> objectMetaDataTable;
	uintptr_t objectMetaDataTable_cached = 0;

	int64_t objectHeaderStride;
	int64_t objectTypeOffset;
	int64_t objectOffsetOffset;

	//callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// event to fire on mcc state change (just clear the caches)
	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing getObjectAddress caches";
		objectMetaDataTable_cached = 0;
	}


public:
	GetObjectAddressDirectPointer(GameState game, IDIContainer& dicon)
		: mGame(game),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); })
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		objectMetaDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectMetaDataTable), game);

		objectHeaderStride = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderStride), game).get();
		objectTypeOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectTypeOffset), game).get();
		objectOffsetOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectOffsetOffset), game).get();


	}


	template <typename TemplatedOrCommonObjectType>
	uintptr_t getObjectAddress(Datum entityDatum, TemplatedOrCommonObjectType* outObjectType)
	{
		LOG_ONCE(PLOG_VERBOSE << "getObjectAddress running for the first time");

		if (objectMetaDataTable_cached == 0)
		{
			// resolve cache
			PLOG_INFO << "resolving getObjectAddress caches";
			if (!objectMetaDataTable->resolve(&objectMetaDataTable_cached)) throw HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError()));

		}

		auto ourObjectHeader = objectMetaDataTable_cached + (objectHeaderStride * entityDatum.index);

		if (outObjectType)
		{
			auto tempOutObjectType = getObjectType<TemplatedOrCommonObjectType>(ourObjectHeader + objectTypeOffset);
			PLOG_DEBUG << "as int: " << (int)tempOutObjectType;
			*outObjectType = tempOutObjectType;
		}

		auto* ourObjectPointer = (uintptr_t*)(ourObjectHeader + objectOffsetOffset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "reading object pointer at " << ptr, ptr = ourObjectPointer);
		auto ourObjectAddress = *ourObjectPointer;


		if (IsBadReadPtr((void*)ourObjectAddress, 4)) throw HCMRuntimeException(std::format("getObjectAddress: Bad read at {:X}", ourObjectAddress));
		return ourObjectAddress;
	}


	virtual uintptr_t getObjectAddressCommon(Datum entityDatum, CommonObjectType* outObjectType) override
	{
		return getObjectAddress<CommonObjectType>(entityDatum, outObjectType);
	}

	virtual uintptr_t getObjectAddressTemplated(Datum entityDatum, TemplatedObjectType* outObjectType) override
	{
		return getObjectAddress<TemplatedObjectType>(entityDatum, outObjectType);
	}
	
};



GetObjectAddress::GetObjectAddress(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: 
		pimpl = std::make_unique<GetObjectAddressDataTable<Halo1ObjectType>>(game, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<GetObjectAddressDataTable<Halo2ObjectType>>(game, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<GetObjectAddressDirectPointer<Halo3ObjectType>>(game, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<GetObjectAddressDirectPointer<Halo3ODSTObjectType>>(game, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<GetObjectAddressDirectPointer<HaloReachObjectType>>(game, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<GetObjectAddressDirectPointer<Halo4ObjectType>>(game, dicon);
		break;


	default: throw HCMInitException("not impl yet");
	}


}
GetObjectAddress::~GetObjectAddress() = default;

uintptr_t GetObjectAddress::getObjectAddress(Datum entityDatum) { return pimpl->getObjectAddressNoObj(entityDatum); }

template <typename TemplatedObjectType>
uintptr_t GetObjectAddress::getObjectAddress(Datum entityDatum, TemplatedObjectType* outObjectType) { return pimpl->getObjectAddressTemplated(entityDatum, outObjectType); }

uintptr_t GetObjectAddress::getObjectAddress(Datum entityDatum, CommonObjectType* outObjectType) { return pimpl->getObjectAddressCommon(entityDatum, outObjectType); }


// template instantiantians (spelling is hard)
template <>
uintptr_t GetObjectAddress::getObjectAddress<CommonObjectType>(Datum datum, CommonObjectType* outObjectType) { return pimpl->getObjectAddressCommon(datum, outObjectType); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo1ObjectType>(Datum datum, Halo1ObjectType* outObjectType) { return pimpl->getObjectAddressTemplated(datum, outObjectType); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo2ObjectType>(Datum datum, Halo2ObjectType* outObjectType) { return pimpl->getObjectAddressTemplated(datum, outObjectType); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo3ObjectType>(Datum datum, Halo3ObjectType* outObjectType) { return pimpl->getObjectAddressTemplated(datum, outObjectType); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo3ODSTObjectType>(Datum datum, Halo3ODSTObjectType* outObjectType) { return pimpl->getObjectAddressTemplated(datum, outObjectType); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<HaloReachObjectType>(Datum datum, HaloReachObjectType* outObjectType) { return pimpl->getObjectAddressTemplated(datum, outObjectType); }

template <>
uintptr_t GetObjectAddress::getObjectAddress<Halo4ObjectType>(Datum datum, Halo4ObjectType* outObjectType) { return pimpl->getObjectAddressTemplated(datum, outObjectType); }

//
//// coooooould change the implementation to do the lookup manually. then it could work with multiple kinds of tables..
//
//typedef int32_t enumBitmask;
//
//extern "C" typedef uintptr_t __fastcall getObjectAddressFunc(Datum datum, enumBitmask expectedObjectTypeBitMask);
//
//// Just calls the games inbuilt function for resolving object addreses from datum
//class GetObjectAddress::GetObjectAddressImpl
//{
//	GameState mGame;
//
//
//	// data
//	std::shared_ptr<MultilevelPointer> getObjectAddressFunction;
//
//	template <typename TemplatedObjectType>
//	enumBitmask expectedObjectTypeToBitmask(std::set <TemplatedObjectType> expectedObjectTypes)
//	{
//		// the games function actually takes a bit mask, so you can match against multiple expected object types.
//		if (expectedObjectTypes.empty()) return -1;
//		
//		enumBitmask out;
//		for (auto& expectedObjectType : expectedObjectTypes)
//		{
//			if ((int32_t)expectedObjectType == -1) return -1; // null means catch all
//			out = out | ((int32_t)1 << (int32_t)expectedObjectType); // set bit at nth position where n = expectedObjectType
//		}
//		return out;
//	}
//
//public:
//
//	template <typename TemplatedObjectType>
//	uintptr_t getObjectAddress(Datum datum, std::set < TemplatedObjectType> expectedObjectTypes)
//	{
//		LOG_ONCE(PLOG_DEBUG << "getObjectAddress");
//
//		// resolve pointer to games getObjectAddressFunction
//		uintptr_t p_getObjectAddressFunc;
//		if (!getObjectAddressFunction->resolve(&p_getObjectAddressFunc)) throw HCMRuntimeException(std::format("could not resolve games getObjectAddressFunc: {}", MultilevelPointer::GetLastError()));
//
//		LOG_ONCE_CAPTURE(PLOG_DEBUG << "p_getObjectAddressFunc: " << pget, pget = p_getObjectAddressFunc);
//
//		LOG_ONCE(PLOG_DEBUG << "calling games getObjectAddressFunc");
//		
//		auto result = ((getObjectAddressFunc*)p_getObjectAddressFunc)(datum, expectedObjectTypeToBitmask<TemplatedObjectType>(expectedObjectTypes));
//		if (!result) throw HCMRuntimeException(std::format("null object address for datum: {:X}, with bitmask {}", datum.operator unsigned int(), expectedObjectTypeToBitmask<TemplatedObjectType>(expectedObjectTypes)));
//		return result;
//	}
//
//	GetObjectAddressImpl(GameState game, IDIContainer& dicon) 
//		: mGame(game)
//	{
//		getObjectAddressFunction = dicon.Resolve<PointerManager>().lock()->getData<std::shared_ptr<MultilevelPointer>>(nameof(getObjectAddressFunction), game);
//	}
//};
//
//GetObjectAddress::GetObjectAddress(GameState game, IDIContainer& dicon) : pimpl(std::make_unique<GetObjectAddressImpl>(game, dicon)) {}
//GetObjectAddress::~GetObjectAddress() = default;
//
//
//uintptr_t GetObjectAddress::getObjectAddress(Datum d, std::set < CommonObjectType> o) { return pimpl->getObjectAddress<CommonObjectType>(d, o); }
//template <typename TemplatedObjectType>
//uintptr_t GetObjectAddress::getObjectAddress(Datum d, std::set < TemplatedObjectType> o) { return pimpl->getObjectAddress<TemplatedObjectType>(d, o); }
//
//
//template <>
//uintptr_t GetObjectAddress::getObjectAddress<Halo1ObjectType>(Datum d, std::set<Halo1ObjectType> o) { return pimpl->getObjectAddress< Halo1ObjectType>(d, o); }
//
//template <>
//uintptr_t GetObjectAddress::getObjectAddress<Halo2ObjectType>(Datum d, std::set < Halo2ObjectType> o) { return pimpl->getObjectAddress< Halo2ObjectType>(d, o); }
//
//template <>
//uintptr_t GetObjectAddress::getObjectAddress<Halo3ObjectType>(Datum d, std::set < Halo3ObjectType> o) { return pimpl->getObjectAddress< Halo3ObjectType>(d, o); }
//
//template <>
//uintptr_t GetObjectAddress::getObjectAddress<Halo3ODSTObjectType>(Datum d, std::set < Halo3ODSTObjectType> o) { return pimpl->getObjectAddress< Halo3ODSTObjectType>(d, o); }
//
//template <>
//uintptr_t GetObjectAddress::getObjectAddress<HaloReachObjectType>(Datum d, std::set < HaloReachObjectType> o) { return pimpl->getObjectAddress< HaloReachObjectType>(d, o); }
//
//template <>
//uintptr_t GetObjectAddress::getObjectAddress<Halo4ObjectType>(Datum d, std::set < Halo4ObjectType> o) { return pimpl->getObjectAddress< Halo4ObjectType>(d, o); }
//
//
