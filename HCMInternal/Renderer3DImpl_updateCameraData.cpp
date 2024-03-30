#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"


// This function is only used as a fallback if we don't have the games actual vertical FOV pointer
template<GameState::Value mGame>
float horizontalFOVToVerticalFOV(float horizontalFOVMeasured, float screenWidth, float screenHeight)
{
	// https://docs.google.com/document/d/1QXaMTPOpLmJP_YWYV_U3g1eMG50cd595fd0_-B3_Ebk/edit
	if constexpr (mGame == GameState::Value::Halo1)
	{
		// very close to perfect. this is essentially H16ML9 from above documentation
		float verticalFOVIn169 = 2 * std::atan(std::tan(horizontalFOVMeasured / 2) * (1080 / 1920.f));
		float actualHorizontal = 2 * std::atan(std::tan(verticalFOVIn169 / 2) * (screenWidth / screenHeight));

		return 2 * std::atan(std::tan(actualHorizontal / 2) * (screenHeight / screenWidth));
	}
	else if constexpr (mGame == GameState::Value::Halo2)
	{
		// fairly close to perfect
		return  2 * std::atan(std::tan(horizontalFOVMeasured / 2) * (screenHeight / screenWidth));
	}
	else
	{
		return  2 * std::atan(std::tan(horizontalFOVMeasured / 2) * (screenHeight / screenWidth));
	}
}



template<GameState::Value mGame>
bool Renderer3DImpl<mGame>::updateCameraData(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 screenSizeIn, ID3D11RenderTargetView* pMainRenderTargetView)
{
	// TODO: currently having some safety issues - specifically if the user quits to main menu in the middle of this function being called, 
	// various bad things will happen (mostly bad ptr reads but that means a crash). Not sure what the best way to guard against this is. 
	// Problem is that the order of MCCStateChanged func calls are random as far as we're concerned, 
	// so RenderImpl might recieve it either BEFORE or AFTER a consumer (ie Waypoint3D) recieves it. 

	try
	{
		lockOrThrow(getGameCameraDataWeak, getGameCameraData);

		auto cameraData = getGameCameraData->getGameCameraData();
		// importantly we quickly copy off in case of game quit-out
		this->cameraPosition = *cameraData.position;
		this->cameraDirection = *cameraData.lookDirForward;
		this->cameraUp = *cameraData.lookDirUp;


		float aspectRatio = screenSizeIn.x / screenSizeIn.y; // aspect ratio is width div height!


		float verticalFov;
		if (this->pVerticalFOVCached != nullptr)
		{
			if (IsBadReadPtr(this->pVerticalFOVCached, 4))
				throw HCMRuntimeException(std::format("Bad pVerticalFOVCached read at: {}", (uintptr_t)this->pVerticalFOVCached));
			verticalFov = *this->pVerticalFOVCached;
		}
		else
		{
			verticalFov = horizontalFOVToVerticalFOV<mGame>(*cameraData.FOV, screenSizeIn.x, screenSizeIn.y);
		}
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "pVerticalFOVCached: " << cached, cached = (uintptr_t)this->pVerticalFOVCached);

		if (XMScalarNearEqual(verticalFov, 0.0f, 0.00001f * 2.0f))
			throw HCMRuntimeException(std::format("Bad vertical FOV value: {}", verticalFov));

#ifdef HCM_DEBUG
		static float adjustFactor = 1.f;

		if (GetKeyState('3') & 0x8000)
		{
			adjustFactor += 0.001f;
		}
		if (GetKeyState('4') & 0x8000)
		{
			adjustFactor -= 0.001f;
		}

		if (GetKeyState('5') & 0x8000)
		{
			PLOG_VERBOSE << "horizontalFov: " << *cameraData.FOV;
			PLOG_VERBOSE << "verticalFov: " << verticalFov;
			PLOG_VERBOSE << "adjustedFov: " << (verticalFov * adjustFactor);
			PLOG_VERBOSE << "pCameraData->horizontalFov: " << std::hex << (uintptr_t)cameraData.FOV;
			PLOG_VERBOSE << "adjustFactor: " << adjustFactor;
		}


		verticalFov = verticalFov * adjustFactor;
#endif


		this->projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(verticalFov, aspectRatio, 0.001f, FAR_CLIP_3D);
		auto lookAt = this->cameraPosition + this->cameraDirection;
		this->viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(this->cameraPosition, lookAt, this->cameraUp);
		this->viewProjectionMatrix = (viewMatrix * projectionMatrix);
		this->pDevice = pDevice;
		this->pDeviceContext = pDeviceContext;
		this->pMainRenderTargetView = pMainRenderTargetView;
		this->screenSize = screenSizeIn;
		this->screenCenter = { screenSize.x / 2.f, screenSize.y / 2.f };

		this->frustumViewWorld.CreateFromMatrix(this->frustumViewWorld, this->projectionMatrix, true); // frustrum in view space
		this->frustumViewWorld.Transform(this->frustumViewWorld, this->viewMatrix.Invert()); // now in world space
		this->frustumViewWorld.GetPlanes((DirectX::XMVECTOR*)&frustumViewWorldPlanes.nearFrustum, (DirectX::XMVECTOR*)&frustumViewWorldPlanes.farFrustum, (DirectX::XMVECTOR*)&frustumViewWorldPlanes.rightFrustum, (DirectX::XMVECTOR*)&frustumViewWorldPlanes.leftFrustum, (DirectX::XMVECTOR*)&frustumViewWorldPlanes.topFrustum, (DirectX::XMVECTOR*)&frustumViewWorldPlanes.bottomFrustum);
		//std::swap(this->frustumViewWorld.Near, this->frustumViewWorld.Far); 		// I have no idea why I have to do this


		if (this->init == false)
		{
			initialise();
			this->init = true;
		}
		return true;
	}
	catch (HCMRuntimeException ex)
	{
		if (this->haveShownError == false)
		{
			this->haveShownError = true;
			ex.append("\nSuppressing further errors");
			runtimeExceptions->handleMessage(ex);
		}
		else
		{
			runtimeExceptions->handleSilent(ex);
		}

		return false;
	}
}


// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;