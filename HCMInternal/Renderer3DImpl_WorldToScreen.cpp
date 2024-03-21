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
IRenderer3D::AppliedClamp Renderer3DImpl<mGame>::clampScreenPositionToEdge(SimpleMath::Vector3& screenPositionOut, SimpleMath::Vector3& world, float clampBorderRatio)
{
	// https://github.com/OBalfaqih/Unity-Tutorials/blob/master/Unity%20Tutorials/WaypointMarker/Scripts/MissionWaypoint.cs#L35

	float edgeBorder = (this->screenSize.x + this->screenSize.y) / 2.f * clampBorderRatio;
	float minX = edgeBorder;
	float maxX = this->screenSize.x - edgeBorder;
	float minY = edgeBorder;
	float maxY = this->screenSize.y - edgeBorder;

	SimpleMath::Vector3 targetDir = world - this->cameraPosition;
	float dot = targetDir.Dot(this->cameraDirection);
	if (dot < 0)
	{
		if (screenPositionOut.x < (this->screenSize.x / 2)) 	// Check if the target is on the left side of the screen
		{
			screenPositionOut.x = maxX; // Place it on the right (Since it's behind the player, it's the opposite)
		}
		else
		{
			screenPositionOut.x = minX; // Place it on the left side
		}

		if (screenPositionOut.y < (this->screenSize.y / 2)) 	// Check if the target is on the top side of the screen
		{
			screenPositionOut.y = maxY; // Place it on the bottom (Since it's behind the player, it's the opposite)
		}
		else
		{
			screenPositionOut.y = minY; // Place it on the top
		}
	}

	// clamp to screen
	screenPositionOut.x = std::clamp(screenPositionOut.x, minX, maxX);
	screenPositionOut.y = std::clamp(screenPositionOut.y, minY, maxY);

	if (screenPositionOut.x == minX) 
	{
		return AppliedClamp::Left;
	}
	else if (screenPositionOut.x == maxX)
	{
		return AppliedClamp::Right;
	}
	else if (screenPositionOut.y == minY)
	{
		return AppliedClamp::Top;
	}
	else if (screenPositionOut.y == maxY)
	{
		return AppliedClamp::Bottom;
	}
	else
	{
		return AppliedClamp::None;
	}

}



template<GameState::Value mGame>
SimpleMath::Vector3 Renderer3DImpl<mGame>::worldPointToScreenPosition(SimpleMath::Vector3 world)
{
	LOG_ONCE(PLOG_VERBOSE << "world to screen");
	auto worldPos = SimpleMath::Vector4::Transform({ world.x, world.y, world.z, 1.f },
		SimpleMath::Matrix::CreateWorld(
			SimpleMath::Vector3::Zero,
			SimpleMath::Vector3::Forward,
			SimpleMath::Vector3::Up
		));

	auto clipSpace = SimpleMath::Vector4::Transform(worldPos, (this->viewMatrix * this->projectionMatrix));

	// result above is actually in homogenous space, divide all components by w to get clipspace.

	clipSpace = clipSpace / clipSpace.w;




	// convert from clipSpace (-1..+1) to screenSpace (0..Width, 0..Height)
	auto out = SimpleMath::Vector3
	(
		(1.f + clipSpace.x) * 0.5 * this->screenSize.x,
		(1.f - clipSpace.y) * 0.5 * this->screenSize.y,
		clipSpace.z
	);





#ifdef HCM_DEBUG
	if (GetKeyState('9') & 0x8000)
	{
		PLOG_VERBOSE << "world to screen outputting: " << out;
	}
#endif

	LOG_ONCE_CAPTURE(PLOG_VERBOSE << "world to screen outputting: " << ss, ss = out);
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
		this->pDevice = pDevice;
		this->pDeviceContext = pDeviceContext;
		this->pMainRenderTargetView = pMainRenderTargetView;
		this->screenSize = screenSizeIn;

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