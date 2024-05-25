#include "pch.h"
#include "ObjectTableRange.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"
#include "IMCCStateHook.h"

// implementation is similiar to GetObjectAddress in many ways




class ObjectTableRangeDataTable : public IObjectTableRangeImpl
{
private:
	GameState mGame;

	//data
	std::shared_ptr<MultilevelPointer> objectMetaDataTable;
	std::shared_ptr<MultilevelPointer> objectDataTable;


	int64_t objectHeaderTableSizeOffset;
	int64_t objectHeaderStride;
	int64_t objectTypeOffset;
	int64_t objectOffsetOffset;

	//callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;


	// event to fire on mcc state change (just clear the caches)
	void onGameStateChange(const MCCState&)
	{
		//PLOG_INFO << "clearing getObjectAddress caches";
	}


public:
	ObjectTableRangeDataTable(GameState game, IDIContainer& dicon)
		: mGame(game),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		objectMetaDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectMetaDataTable), game);

		objectHeaderStride = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderStride), game).get();
		objectTypeOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectTypeOffset), game).get();
		objectOffsetOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectOffsetOffset), game).get();
		objectHeaderTableSizeOffset = * ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderTableSizeOffset), game).get();
		objectDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectDataTable), game);

	}

	// TODO: cache entire object table each game tick
	virtual std::vector<ObjectInfo> getObjectTableRange() override
	{
		uintptr_t objectMetaDataTable_resolved = 0;
		uintptr_t objectDataTable_resolved = 0;
	

		// resolve cache
		if (!objectMetaDataTable->resolve(&objectMetaDataTable_resolved)) throw HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError()));
		if (!objectDataTable->resolve(&objectDataTable_resolved)) throw HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError()));

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "objectMetaDataTable_resolved: " << std::hex << o, o = objectMetaDataTable_resolved);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "objectDataTable_resolved: " << std::hex << o, o = objectDataTable_resolved);


		uint16_t tableMaxSize = *(uint16_t*)(objectMetaDataTable_resolved + objectHeaderTableSizeOffset);



		std::vector<ObjectInfo> finalOut;
		for (uint16_t objectIndex = 0; objectIndex < tableMaxSize; objectIndex++)
		{


			auto ourObjectHeader = objectMetaDataTable_resolved + (objectHeaderStride * objectIndex);
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "pOurObjectHeader" << std::hex << p, p = ourObjectHeader);
			ObjectInfo out;

			// first two bytes at header are datum salt
			out.objectDatum.salt = *(uint16_t*)ourObjectHeader;
			out.objectDatum.index = objectIndex;
			out.objectType = *(CommonObjectType*)(ourObjectHeader + objectTypeOffset);


			auto ourObjectOffset = *(DWORD*)(ourObjectHeader + objectOffsetOffset);


			if (ourObjectOffset == 0xFFFFFFFF)
			{
				tableMaxSize++;
				continue;
			}
			out.objectAddress = objectDataTable_resolved + ourObjectOffset;

			finalOut.push_back(out);
		}
		return finalOut;
		

	}
};


class ObjectTableRangeDirectPointer : public IObjectTableRangeImpl
{
private:

	GameState mGame;

	//data
	std::shared_ptr<MultilevelPointer> objectMetaDataTable;
	uintptr_t objectMetaDataTable_cached = 0;

	int64_t objectHeaderTableSizeOffset;
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
	ObjectTableRangeDirectPointer(GameState game, IDIContainer& dicon)
		: mGame(game),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		objectMetaDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectMetaDataTable), game);

		objectHeaderTableSizeOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderTableSizeOffset), game).get();
		objectHeaderStride = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderStride), game).get();
		objectTypeOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectTypeOffset), game).get();
		objectOffsetOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectOffsetOffset), game).get();


	}

	virtual std::vector<ObjectInfo> getObjectTableRange() override
	{
		// TODO: cache value each game tick
		if (objectMetaDataTable_cached == 0)
		{
			// resolve cache
			PLOG_INFO << "resolving getObjectAddress caches";
			if (!objectMetaDataTable->resolve(&objectMetaDataTable_cached)) throw HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError()));

		}

		uint16_t tableMaxSize = *(uint16_t*)(objectMetaDataTable_cached + objectHeaderTableSizeOffset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "getObjectTableRange::tableMaxSize: " << s, s = tableMaxSize);

		std::vector<ObjectInfo> finalOut;
		for (uint16_t objectIndex = 0; objectIndex < tableMaxSize; objectIndex++)
		{
			auto ourObjectHeader = objectMetaDataTable_cached + (objectHeaderStride * objectIndex);
			ObjectInfo out;

			// first two bytes at header are datum salt
			out.objectDatum.salt = *(uint16_t*)ourObjectHeader;
			out.objectDatum.index = objectIndex;
			out.objectType = *(CommonObjectType*)(ourObjectHeader + objectTypeOffset);


			auto* ourObjectPointer = (uintptr_t*)(ourObjectHeader + objectOffsetOffset);
			out.objectAddress = *ourObjectPointer;

			finalOut.push_back(out);
		}
		return finalOut;

	}
};


ObjectTableRange::ObjectTableRange(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<ObjectTableRangeDataTable>(game, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<ObjectTableRangeDataTable>(game, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<ObjectTableRangeDirectPointer>(game, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<ObjectTableRangeDirectPointer>(game, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<ObjectTableRangeDirectPointer>(game, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<ObjectTableRangeDirectPointer>(game, dicon);
		break;


	default: throw HCMInitException("not impl yet");
	}
}

ObjectTableRange::~ObjectTableRange() = default;

std::vector<ObjectInfo> ObjectTableRange::getObjectTableRange() { return pimpl->getObjectTableRange(); }