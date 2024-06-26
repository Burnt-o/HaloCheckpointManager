#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "Datum.h"
#include "IMCCStateHook.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "GameTickEventHook.h"
#include "IMakeOrGetCheat.h"
#include "GameObjectPool.h"

class IObjectPoolFactory : public IGameObjectFactory, public IGameObjectMetaFactory
{
public:
	virtual uint16_t resolveSize() = 0;

	virtual ~IObjectPoolFactory() = default;
};

class H1GameObjectMeta : public IGameObjectMeta
{
	virtual const CommonObjectType* objectType() override { return };
	virtual const Datum* objectDatum() = 0;
};

class GameObjectPoolFactoryH1 : public IObjectPoolFactory
{
private:
	//callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	//data
	std::shared_ptr<MultilevelPointer> objectMetaDataTable;
	std::shared_ptr<MultilevelPointer> objectDataTable;

	int64_t objectHeaderTableSizeOffset;
	int64_t objectHeaderStride;
	int64_t objectTypeOffset;
	int64_t objectOffsetOffset;

	// cached data
	uintptr_t objectMetaDataTable_resolved;
	uintptr_t objectDataTable_resolved;
	uint16_t tableMaxSize;
	bool addressCacheIsUpToDate = false;

public:
	GameObjectPoolFactoryH1(GameState game, IDIContainer& dicon)
		: MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { addressCacheIsUpToDate = false; })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		objectMetaDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectMetaDataTable), game);

		objectHeaderStride = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderStride), game).get();
		objectTypeOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectTypeOffset), game).get();
		objectOffsetOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectOffsetOffset), game).get();
		objectHeaderTableSizeOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(objectHeaderTableSizeOffset), game).get();
		objectDataTable = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(objectDataTable), game);
	}

	std::expected<void, HCMRuntimeException> updateCachedData()
	{
		// resolve cache
		if (!objectMetaDataTable->resolve(&objectMetaDataTable_resolved)) return std::unexpected(HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError())));
		if (!objectDataTable->resolve(&objectDataTable_resolved)) return std::unexpected(HCMRuntimeException(std::format("Failed to resolve objectMetaDataTable: {}", MultilevelPointer::GetLastError())));

		auto* pTableMaxSize = (uint16_t*)(objectMetaDataTable_resolved + objectHeaderTableSizeOffset);

		if (IsBadReadPtr(pTableMaxSize, sizeof(uint16_t)))  return std::unexpected(HCMRuntimeException(std::format("Bad read of pTableMaxSize: 0x{}", (uintptr_t)pTableMaxSize)));
		uint16_t tableMaxSize = *pTableMaxSize;

		addressCacheIsUpToDate = true;
	}


	virtual uint16_t resolveSize() override
	{
		if (!addressCacheIsUpToDate)
		{
			auto cacheSuccessfullyUpdated = updateCachedData();
			if (!cacheSuccessfullyUpdated) throw cacheSuccessfullyUpdated.error();
		}

		return tableMaxSize;
	}

	virtual std::shared_ptr<IGameObject> makeObject(const Datum* datum) override
	{
		if (!addressCacheIsUpToDate)
		{
			auto cacheSuccessfullyUpdated = updateCachedData();
			if (!cacheSuccessfullyUpdated) throw cacheSuccessfullyUpdated.error();
		}


	}

	virtual std::shared_ptr<IGameObjectMeta> makeObjectMeta(uint16_t index) override
	{
		if (!addressCacheIsUpToDate)
		{
			auto cacheSuccessfullyUpdated = updateCachedData();
			if (!cacheSuccessfullyUpdated) throw cacheSuccessfullyUpdated.error();
		}


	}
};


class GameObjectPoolImpl : public IProvideObjectPoolContainer
{
private:
	GameState mGame;

	std::shared_ptr< IObjectPoolFactory> objectPoolFactory;

	// cached to gametick
	std::optional<std::pair<ObjectPoolContainer, uint32_t>> objectPoolCache;


	// injected services
	std::weak_ptr< GameTickEventHook> gameTickEventHookWeak;




public:
	GameObjectPoolImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook))
	{

		switch (game)
		{
		case GameState::Value::Halo1:
			objectPoolFactory = std::make_shared< GameObjectPoolFactoryH1>(game, dicon);
			break;

		//case GameState::Value::Halo2:
		//	objectPoolFactory = std::make_shared< GameObjectPoolFactoryH1>(game, dicon);
		//	break;

		//case GameState::Value::Halo3:
		//	objectPoolFactory = std::make_shared< GameObjectPoolFactoryH1>(game, dicon);
		//	break;

		//case GameState::Value::Halo3ODST:
		//	objectPoolFactory = std::make_shared< GameObjectPoolFactoryH1>(game, dicon);
		//	break;

		//case GameState::Value::HaloReach:
		//	objectPoolFactory = std::make_shared< GameObjectPoolFactoryH1>(game, dicon);
		//	break;

		//case GameState::Value::Halo4:
		//	objectPoolFactory = std::make_shared< GameObjectPoolFactoryH1>(game, dicon);
		//	break;

		default:
			throw HCMInitException("ObjectPoolFactory not impl yet for this game");
		}


	}

	


	void updateObjectPoolCache()
	{
	
		lockOrThrow(gameTickEventHookWeak, gameTickEventHook);
		auto currentTick = gameTickEventHook->getCurrentGameTick();
		if (objectPoolCache.has_value() && objectPoolCache.value().second == currentTick)
		{
			
		}
		else
		{
			objectPoolCache = std::pair<ObjectPoolContainer, uint32_t>(ObjectPoolContainer(objectPoolFactory, objectPoolFactory->resolveSize()), currentTick);
		}
	}

	virtual ObjectPoolContainer::iterator begin() override
	{
		updateObjectPoolCache();
		return objectPoolCache.value().first.begin();
	}
	virtual ObjectPoolContainer::iterator end() override 
	{ 
		updateObjectPoolCache();
		return objectPoolCache.value().first.end();
	}

	virtual bool empty() override 
	{  
		updateObjectPoolCache();
		return objectPoolCache.value().first.empty();
	}

	virtual uint16_t size() override 
	{
		updateObjectPoolCache();
		return objectPoolCache.value().first.size();
	}




};


GameObjectPool::GameObjectPool(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique<GameObjectPoolImpl>(game, dicon);
}