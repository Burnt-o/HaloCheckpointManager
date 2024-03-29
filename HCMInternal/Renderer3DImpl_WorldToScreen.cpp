#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"

// https://docs.google.com/document/d/1QXaMTPOpLmJP_YWYV_U3g1eMG50cd595fd0_-B3_Ebk/edit
template<GameState::Value mGame>
float horizontalFOVToVerticalFOV(float horizontalFOVMeasured, float screenWidth, float screenHeight)
{

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
bool Renderer3DImpl<mGame>::pointOnScreen(const SimpleMath::Vector3& worldPointPosition)
{
	return this->frustumViewWorld.Contains(worldPointPosition) != DirectX::ContainmentType::DISJOINT;
}

template<GameState::Value mGame>
bool Renderer3DImpl<mGame>::clampScreenPositionToEdge(SimpleMath::Vector3& screenPositionOut, SimpleMath::Vector3& worldPointPosition)
{
	if (pointOnScreen(worldPointPosition)) return false;

	 //check frustrum for interesection with line from world pos to camera pos
	SimpleMath::Vector3 worldToCameraDir = this->cameraPosition + this->cameraDirection - worldPointPosition;
	float intersectionDistance;
	if (this->frustumViewWorld.Intersects(worldPointPosition, worldToCameraDir, intersectionDistance))
	{
		// find where the ray touches the frustrum
		auto clampedWorldPosition = worldPointPosition + (worldToCameraDir * intersectionDistance);
		// that's the new screen position
		screenPositionOut = worldPointToScreenPosition(clampedWorldPosition);
		return true;
	}
	else
	{
		// this *shouldn't* happen;
		LOG_ONCE(PLOG_ERROR << "point off screen but not interesecting with frustrum - weird");
		return false;
	}


}



template<GameState::Value mGame>
SimpleMath::Vector3 Renderer3DImpl<mGame>::worldPointToScreenPosition(SimpleMath::Vector3 worldPointPosition)
{
	LOG_ONCE(PLOG_VERBOSE << "world to screen");
	auto worldPosTransformed = SimpleMath::Vector4::Transform({ worldPointPosition.x, worldPointPosition.y, worldPointPosition.z, 1.f },
		SimpleMath::Matrix::CreateWorld(
			SimpleMath::Vector3::Zero,
			SimpleMath::Vector3::Forward,
			SimpleMath::Vector3::Up
		));

	auto clipSpace = SimpleMath::Vector4::Transform(worldPosTransformed, viewProjectionMatrix);

	// result above is actually in homogenous space, divide all components by w to get clipspace.
	clipSpace = clipSpace / clipSpace.w;

	// convert from clipSpace (-1..+1) to screenSpace (0..Width, 0..Height)
	auto out = SimpleMath::Vector3
	(
		(1.f + clipSpace.x) * 0.5 * this->screenSize.x,
		(1.f - clipSpace.y) * 0.5 * this->screenSize.y,
		clipSpace.z
	);

	return out;
}

template<GameState::Value mGame>
float Renderer3DImpl<mGame>::cameraDistanceToWorldPoint(SimpleMath::Vector3 worldPointPosition)
{
	return SimpleMath::Vector3::Distance(this->cameraPosition, worldPointPosition);
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

SimpleMath::Vector2 get2DScreenPos(std::pair<SimpleMath::Vector3, bool>& in)
{
	return { in.first.x, in.first.y };
}

template<GameState::Value mGame>
void Renderer3DImpl<mGame>::renderTriggerModel(TriggerModel& model)
{
	SimpleMath::Vector3 cornersWorldPos [8];
	model.box.GetCorners(cornersWorldPos);
	// TODO: cache getCorners result since it recalculates each time (probably no biggie tho)
	// TODO: check if bounding box intersects with view frustrum for clipping

	// bool = visible
	SimpleMath::Vector2 cornersScreenPos[8];
	bool cornersVisible[8];

	for (int i = 0; i < 8; i++)
	{
		auto screenPos = worldPointToScreenPosition(cornersWorldPos[i]);
		cornersScreenPos[i] = { screenPos.x, screenPos.y };
		//auto clamped = clampScreenPositionToEdge(screenPos, cornersWorldPos[i]);
		cornersVisible[i] = pointOnScreen(cornersWorldPos[i]);
	}

	
	
	

	//XMGLOBALCONST XMVECTORF32 g_BoxOffset[8] =
	//{
	//	{ { { -1.0f, -1.0f,  1.0f, 0.0f } } }, 0 0 1	0
	//	{ { {  1.0f, -1.0f,  1.0f, 0.0f } } }, 1 0 1	1
	//	{ { {  1.0f,  1.0f,  1.0f, 0.0f } } }, 1 1 1	2
	//	{ { { -1.0f,  1.0f,  1.0f, 0.0f } } }, 0 1 1	3
	//	{ { { -1.0f, -1.0f, -1.0f, 0.0f } } }, 0 0 0	4
	//	{ { {  1.0f, -1.0f, -1.0f, 0.0f } } }, 1 0 0	5
	//	{ { {  1.0f,  1.0f, -1.0f, 0.0f } } }, 1 1 0	6
	//	{ { { -1.0f,  1.0f, -1.0f, 0.0f } } }, 0 1 0	7
	//};




	ImU32 filledColor = 0xFF0000A0;
	ImU32 outlineColor = 0xFF0000FF;

	// TODO: check if any point of a face is visible before rendering.
	// But how to handle case where face is visible but one of the points is behind camera?
	// maybe we need worldToScreen to automatically flip points that are behind the camera. Will need to update clampToScreenEdge to remove that logic.

	// near face
	ImGui::GetForegroundDrawList()->AddQuadFilled(cornersScreenPos[0], cornersScreenPos[1], cornersScreenPos[2], cornersScreenPos[3], filledColor);


	// far face
	ImGui::GetForegroundDrawList()->AddQuadFilled(cornersScreenPos[4], cornersScreenPos[5], cornersScreenPos[6], cornersScreenPos[7], filledColor);


	// uhhhh hope I got these right
	ImGui::GetForegroundDrawList()->AddQuadFilled(cornersScreenPos[0], cornersScreenPos[1], cornersScreenPos[5], cornersScreenPos[4], filledColor);

	ImGui::GetForegroundDrawList()->AddQuadFilled(cornersScreenPos[0], cornersScreenPos[3], cornersScreenPos[7], cornersScreenPos[4], filledColor);

	ImGui::GetForegroundDrawList()->AddQuadFilled(cornersScreenPos[1], cornersScreenPos[2], cornersScreenPos[6], cornersScreenPos[5], filledColor);

	ImGui::GetForegroundDrawList()->AddQuadFilled(cornersScreenPos[2], cornersScreenPos[3], cornersScreenPos[7], cornersScreenPos[6], filledColor);
		
	// outline the cube with lines
#define drawLinePairIfVisible(p1, p2) if (cornersVisible[p1] || cornersVisible[p2]) { ImGui::GetForegroundDrawList()->AddLine(cornersScreenPos[p1], cornersScreenPos[p2], outlineColor);}

	// near
	drawLinePairIfVisible(0, 1);
	drawLinePairIfVisible(1, 2);
	drawLinePairIfVisible(2, 3);
	drawLinePairIfVisible(3, 0);

	// far
	drawLinePairIfVisible(4, 5);
	drawLinePairIfVisible(5, 6);
	drawLinePairIfVisible(6, 7);
	drawLinePairIfVisible(7, 4);

	// connect
	drawLinePairIfVisible(0, 4);
	drawLinePairIfVisible(1, 5);
	drawLinePairIfVisible(2, 6);
	drawLinePairIfVisible(3, 7);



}


// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;