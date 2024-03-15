#include "pch.h"
#include "GetGameCameraData.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "DynamicStructFactory.h"
#include "IMCCStateHook.h"

class GetGameCameraData::GetGameCameraDataImpl
{
private:

	// data
	enum class cameraDataFields { position, velocity, FOV, lookDirForward, lookDirUp};
	std::shared_ptr<DynamicStruct<cameraDataFields>> cameraDataStruct;

	std::shared_ptr<MultilevelPointer> cameraDataPointer;


	bool cacheValid = false;
	std::unique_ptr<GameCameraData> cachedCameraData;

	//callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// event to fire on mcc state change (just clear the caches)
	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing GetCameraDataImpl caches";
		cacheValid = false;
	}

public:
	GetGameCameraDataImpl(GameState game, IDIContainer& dicon)
		:
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		cameraDataStruct = DynamicStructFactory::make<cameraDataFields>(ptr, game);
		cameraDataPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(cameraDataPointer), game);
	}


	GameCameraData getGameCameraData()
	{
		if (cacheValid == false)
		{
			uintptr_t cameraDataPointerResolved;
			if (!cameraDataPointer->resolve(&cameraDataPointerResolved)) throw HCMRuntimeException(std::format("Failed to resolve cameraDataPointer: ", MultilevelPointer::GetLastError()));

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "cameraDataPointerResolved: " << std::hex << rs, rs = cameraDataPointerResolved);

			cameraDataStruct->currentBaseAddress = cameraDataPointerResolved;
			cachedCameraData = std::make_unique<GameCameraData>
			(
				cameraDataStruct->field<SimpleMath::Vector3>(cameraDataFields::position),
				cameraDataStruct->field<SimpleMath::Vector3>(cameraDataFields::velocity),
				cameraDataStruct->field<float>(cameraDataFields::FOV),
				cameraDataStruct->field<SimpleMath::Vector3>(cameraDataFields::lookDirForward),
				cameraDataStruct->field<SimpleMath::Vector3>(cameraDataFields::lookDirUp)
			);

			cacheValid = true;
		}

		return *cachedCameraData.get();
	}
};


GetGameCameraData::GetGameCameraData(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< GetGameCameraDataImpl>(gameImpl, dicon);
}
GetGameCameraData::~GetGameCameraData()
{
	PLOG_DEBUG << "~" << getName();
}

GameCameraData GetGameCameraData::getGameCameraData() { return pimpl->getGameCameraData(); }