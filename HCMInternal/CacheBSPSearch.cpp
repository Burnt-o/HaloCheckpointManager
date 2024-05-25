#include "pch.h"
#include "CacheBSPSearch.h"
#include "SettingsStateAndEvents.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"

#ifdef CACHE_BSPS

template<typename T, size_t N>
class PreallocatedArray
{
private:
	T size = 0;
	std::array<T, N> data;
public:
	// r5
	~PreallocatedArray() = default;

	PreallocatedArray(const PreallocatedArray& other) // copy constructor 
	{
		//memcpy(data.begin(), other.begin(), other.size * sizeof(T));
		std::copy(other.data.begin(), other.data.begin() + other.size, data.begin());
		size = other.size;
	}

	PreallocatedArray(PreallocatedArray&& other) noexcept // move constructor
	{
		std::copy(other.data.begin(), other.data.begin() + other.size, data.begin());
		// we don't bother to zerofill the source array, just set the size to 0
		size = std::exchange(other.size, 0);
	}


	PreallocatedArray& operator=(const PreallocatedArray& other) // copy assignment constructor
	{
		if (this == &other)
			return *this;

		*this = PreallocatedArray(other);
		return *this;
	}


	PreallocatedArray& operator=(PreallocatedArray&& other) noexcept // move assignment
	{
		std::copy(other.data.begin(), other.data.begin() + other.size, data.begin());
		// we don't bother to zerofill the source array, just set the size to 0
		size = std::exchange(other.size, 0);
		return *this;
	}


};

struct VertexCollisionInfo
{
	// i'm not 100% i've got these names right but it's something along these lines
	PreallocatedArray<uint32_t, 1024> potCollVerticeIndices;
	PreallocatedArray<uint32_t, 1024> potCollEdgeIndices;
	PreallocatedArray<uint32_t, 1024> potCollSurfaceIndices;
	PreallocatedArray<uint32_t, 1024> potCollPlaneIndices;

};

static_assert(sizeof(VertexCollisionInfo) == (1025 * sizeof(uint32_t) * 4));

struct CollisionRequest
{
	uintptr_t pScenarioStructureBSP;
	uint16_t maxLeafDepth;
	uint16_t something;
	uintptr_t unknownStackStruct1;
	uint32_t unknown2;
	SimpleMath::Vector3* pEntityPosition;
	float PositiveCollisionRadius;
	uint32_t unknownNull;
	VertexCollisionInfo* outVertexInfo;
	uint32_t currentOutLeafIndex;
	uint32_t LeafArrayOffset;
	uint32_t outLeafArray[127];
	uint32_t outFrontOrBackEnum;
	uint32_t moreFlags;


};

struct CachedData
{
	char returnResult;
	VertexCollisionInfo vertexCollisionInfo;
	CachedData(char r, VertexCollisionInfo v) : returnResult(r), vertexCollisionInfo(v) {}
};





class CacheBSPSearch::CacheBSPSearchImpl
{
private:

	static inline CacheBSPSearchImpl* instance = nullptr;

	std::shared_ptr<ModuleInlineHook> findEntityInBSPTreeInlineHook;


	std::map<uintptr_t, CachedData> multiCachedData;

	ScopedCallback<ToggleEvent> toggleCallback;
	void onToggleCallback(bool& n)
	{
		multiCachedData.clear();

		findEntityInBSPTreeInlineHook->setWantsToBeAttached(n);
		
	}


	typedef char (*FindEntityInBSPTree)(uintptr_t pSBSP, uint32_t maxTreeDepth, __int64 heapPointer, SimpleMath::Vector3* entityPos, float zeroFourTwoF, VertexCollisionInfo* outVertexCollisionInfo);



	static char __fastcall findEntityInBSPTreeInlineHookFunction(uintptr_t pBSPTree, uint32_t maxTreeDepth, __int64 heapPointer, SimpleMath::Vector3* entityPos, float zeroFourTwoF, VertexCollisionInfo* outVertexCollisionInfo)
	{


		if (!instance->multiCachedData.contains(pBSPTree))
		{
			auto result = instance->findEntityInBSPTreeInlineHook->getInlineHook().original<FindEntityInBSPTree>()(pBSPTree, maxTreeDepth, heapPointer, entityPos, 4.f, outVertexCollisionInfo);
			CachedData cachedData(result, *outVertexCollisionInfo);
			instance->multiCachedData.insert_or_assign(pBSPTree, cachedData);
		}


		auto& cachedData = instance->multiCachedData.at(pBSPTree);
		*outVertexCollisionInfo = cachedData.vertexCollisionInfo;
		return cachedData.returnResult;




		return instance->findEntityInBSPTreeInlineHook->getInlineHook().original<FindEntityInBSPTree>()(pBSPTree, maxTreeDepth, heapPointer, entityPos, zeroFourTwoF, outVertexCollisionInfo);


	}

public:
	CacheBSPSearchImpl(GameState gameImpl, IDIContainer& dicon)
		:
		toggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->cacheBSPSearchToggle->valueChangedEvent, [this](bool& n) {onToggleCallback(n); })
	{
		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		//auto setCacheMidhookFunction = ptr->getData < std::shared_ptr< MultilevelPointer>>(nameof(setCacheMidhookFunction), gameImpl);
		//setCacheMidhook = ModuleMidHook::make(gameImpl.toModuleName(), setCacheMidhookFunction, onSetCacheMidhook);

		auto findEntityInBSPTreeInlineFunction = ptr->getData < std::shared_ptr< MultilevelPointer>>(nameof(findEntityInBSPTreeInlineFunction), gameImpl);
		findEntityInBSPTreeInlineHook = ModuleInlineHook::make(gameImpl.toModuleName(), findEntityInBSPTreeInlineFunction, findEntityInBSPTreeInlineHookFunction);


		//TODO need destruction guard

	}
};

#endif








CacheBSPSearch::CacheBSPSearch(GameState gameImpl, IDIContainer& dicon)
{
#ifdef CACHE_BSPS
	pimpl = std::make_unique< CacheBSPSearchImpl>(gameImpl, dicon);
#endif
}

CacheBSPSearch::~CacheBSPSearch() = default;