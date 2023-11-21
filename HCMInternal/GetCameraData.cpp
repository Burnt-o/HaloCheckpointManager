#include "pch.h"
#include "GetCameraData.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "DynamicStructFactory.h"
#include "IMCCStateHook.h"

class GetCameraData::GetCameraDataImpl
{
private:

	// data
	enum class cameraDataFields { position, velocity, FOV, lookDirForward, lookDirUp};
	std::shared_ptr<DynamicStruct<cameraDataFields>> cameraDataStruct;

	std::shared_ptr<MultilevelPointer> cameraDataPointer;


	bool cacheValid = false;
	std::unique_ptr<CameraDataPtr> cachedCameraData;

	//callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// event to fire on mcc state change (just clear the caches)
	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing GetCameraDataImpl caches";
		cacheValid = false;
	}

public:
	GetCameraDataImpl(GameState game, IDIContainer& dicon) 
		:
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); })
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		cameraDataStruct = DynamicStructFactory::make<cameraDataFields>(ptr, game);
		cameraDataPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(cameraDataPointer), game);
	}


	CameraDataPtr getCameraData()
	{
		if (cacheValid == false)
		{
			uintptr_t cameraDataPointerResolved;
			if (!cameraDataPointer->resolve(&cameraDataPointerResolved)) throw HCMRuntimeException("Failed to resolve cameraDataPointer");

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "cameraDataPointerResolved: " << std::hex << rs, rs = cameraDataPointerResolved);

			cameraDataStruct->currentBaseAddress = cameraDataPointerResolved;
			cachedCameraData = std::make_unique<CameraDataPtr>
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


GetCameraData::GetCameraData(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< GetCameraDataImpl>(gameImpl, dicon);
}
GetCameraData::~GetCameraData()
{
	PLOG_DEBUG << "~" << getName();
}

CameraDataPtr GetCameraData::getCameraData() { return pimpl->getCameraData(); }