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
	enum class cameraDataFields { position, velocity, forwardLookDir, upLookDir, FOV };
	std::shared_ptr<DynamicStruct<cameraDataFields>> cameraDataStruct;

	std::shared_ptr<MultilevelPointer> cameraDataPointer;


	bool cacheValid = false;
	CameraDataPtr cachedCameraData;

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

			cameraDataStruct->currentBaseAddress = cameraDataPointerResolved;
			cachedCameraData.position = cameraDataStruct->field<SimpleMath::Vector3>(cameraDataFields::position);
			cachedCameraData.velocity = cameraDataStruct->field<SimpleMath::Vector3>(cameraDataFields::velocity);
			cachedCameraData.forwardLookDir = cameraDataStruct->field<SimpleMath::Vector3>(cameraDataFields::forwardLookDir);
			cachedCameraData.upLookDir = cameraDataStruct->field<SimpleMath::Vector3>(cameraDataFields::upLookDir);
			cachedCameraData.FOV = cameraDataStruct->field<float>(cameraDataFields::FOV);

			cacheValid = true;
		}

		return cachedCameraData;
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