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
		this->frustumViewWorld.Transform(this->frustumViewWorldBackwards, SimpleMath::Matrix::CreateFromAxisAngle(this->cameraUp, DirectX::XM_PI)) ; // get the frustum but facing backwards so we can check for wrapparounds

		std::array<XMFLOAT3, 8> frustumCorners;
		this->frustumViewWorld.GetCorners(frustumCorners.data());


		this->frustumViewWorld.GetPlanes(nullptr, nullptr, (DirectX::XMVECTOR*)&frustumViewWorldSidePlanes[0], (DirectX::XMVECTOR*)&frustumViewWorldSidePlanes[1], (DirectX::XMVECTOR*)&frustumViewWorldSidePlanes[2], (DirectX::XMVECTOR*)&frustumViewWorldSidePlanes[3]);


		this->frustumViewWorld.GetPlanes((DirectX::XMVECTOR*)&frustumViewWorldPlanes[0], (DirectX::XMVECTOR*)&frustumViewWorldPlanes[1], (DirectX::XMVECTOR*)&frustumViewWorldPlanes[2], (DirectX::XMVECTOR*)&frustumViewWorldPlanes[3], (DirectX::XMVECTOR*)&frustumViewWorldPlanes[4], (DirectX::XMVECTOR*)&frustumViewWorldPlanes[5]);


		//     Near    Far
		//    0----1  4----5
		//    |    |  |    |
		//    |    |  |    |
		//    3----2  7----6
		this->frustumViewWorldFaces[0] = { frustumCorners[0], frustumCorners[1], frustumCorners[2], frustumCorners[3]}; // near
		this->frustumViewWorldFaces[1] = { frustumCorners[4], frustumCorners[5], frustumCorners[6], frustumCorners[7] }; // far
		this->frustumViewWorldFaces[2] = { frustumCorners[0], frustumCorners[4], frustumCorners[5], frustumCorners[1] }; // top
		this->frustumViewWorldFaces[3] = { frustumCorners[3], frustumCorners[7], frustumCorners[6], frustumCorners[2] }; // bottom
		this->frustumViewWorldFaces[4] = { frustumCorners[3], frustumCorners[7], frustumCorners[4], frustumCorners[0] }; // left
		this->frustumViewWorldFaces[5] = { frustumCorners[2], frustumCorners[6], frustumCorners[5], frustumCorners[1] }; // right


#ifdef HCM_DEBUG
		if (GetKeyState('7') & 0x8000)
		{
			debugFrustumViewWorldFaces = frustumViewWorldFaces;
		}

		if (GetKeyState('8') & 0x8000 && debugFrustumViewWorldFaces.has_value())
		{
			renderDebugFrustumFaces();
		}

		if (GetKeyState('9') & 0x8000 && debugFrustumViewWorldFaces.has_value())
		{
			static int faceToRender = 0;
			static int holdForFrames = 0;
			renderDebugFrustumFaces(faceToRender);

			
			if (holdForFrames == 0)
			{
				faceToRender = (faceToRender + 1) % 6;
			}
			holdForFrames = (holdForFrames + 1) % 30;
			
		}


#endif







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