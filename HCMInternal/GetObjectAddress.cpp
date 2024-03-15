#include "pch.h"
#include "GetObjectAddress.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
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
			auto ptr = dicon.Resolve<PointerDataStore>().lock();
			objectMetaDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectMetaDataTable), game);

			objectHeaderStride = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderStride), game).get();
			objectTypeOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectTypeOffset), game).get();
			objectOffsetOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectOffsetOffset), game).get();
			objectDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectDataTable), game);

		}



	virtual bool isValidDatum(Datum entityDatum) override
	{
		if (objectMetaDataTable_cached == 0 || objectDataTable_cached == 0)
		{
			// resolve cache
			PLOG_INFO << "resolving getObjectAddress caches";
			if (!objectMetaDataTable->resolve(&objectMetaDataTable_cached)) throw HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError()));
			if (!objectDataTable->resolve(&objectDataTable_cached)) throw HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError()));
		}

		auto ourObjectHeader = objectMetaDataTable_cached + (objectHeaderStride * entityDatum.index);

		// first two bytes at object header are the salt.. confirm they match
		return (*(uint16_t*)ourObjectHeader == entityDatum.salt);

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

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "ourObjectHeader: " << ooh, ooh = ourObjectHeader);

		// first two bytes at object header are the salt.. confirm they match
		if (*(uint16_t*)ourObjectHeader != entityDatum.salt) throw HCMRuntimeException(std::format("Mismatched datum salt! expected: {:X}, observed: {:X}", entityDatum.salt, *(uint16_t*)ourObjectHeader));


		if (outObjectType)
		{
			auto tempOutObjectType = getObjectType<TemplatedOrCommonObjectType>(ourObjectHeader + objectTypeOffset);
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "as int: " << (int)tObjectType, tObjectType = tempOutObjectType);
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
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		objectMetaDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectMetaDataTable), game);

		objectHeaderStride = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderStride), game).get();
		objectTypeOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectTypeOffset), game).get();
		objectOffsetOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectOffsetOffset), game).get();


	}



	virtual bool isValidDatum(Datum entityDatum) override
	{
		if (objectMetaDataTable_cached == 0)
		{
			// resolve cache
			PLOG_INFO << "resolving getObjectAddress caches";
			if (!objectMetaDataTable->resolve(&objectMetaDataTable_cached)) throw HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError()));

		}

		auto ourObjectHeader = objectMetaDataTable_cached + (objectHeaderStride * entityDatum.index);

		// first two bytes at object header are the salt.. confirm they match
		return (*(uint16_t*)ourObjectHeader == entityDatum.salt);

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

		// TODO:: ADD SALT CHECK, check that it matches what's listed in the header table, throw if not.
		// Maybe also add a bool isDatumValid(Datum, TemplatedOrCommonObjectType) func
		//static_assert(true == false && "DEAR GOD BURNT DON'T FORGET");

		auto ourObjectHeader = objectMetaDataTable_cached + (objectHeaderStride * entityDatum.index);


		// first two bytes at object header are the salt.. confirm they match
		if (*(uint16_t*)ourObjectHeader != entityDatum.salt) throw HCMRuntimeException(std::format("Mismatched datum salt! expected: {:X}, observed: {:X}", entityDatum.salt, *(uint16_t*)ourObjectHeader));


		if (outObjectType)
		{
			auto tempOutObjectType = getObjectType<TemplatedOrCommonObjectType>(ourObjectHeader + objectTypeOffset);
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "as int: " << (int)tObjectType, tObjectType = tempOutObjectType);
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


bool GetObjectAddress::isValidDatum(Datum entityDatum) { return pimpl->isValidDatum(entityDatum); }

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
