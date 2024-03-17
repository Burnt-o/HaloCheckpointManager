#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"

// https://docs.google.com/document/d/1QXaMTPOpLmJP_YWYV_U3g1eMG50cd595fd0_-B3_Ebk/edit
template<GameState::Value mGame>
float horizontalFOVToVerticalFOV(float horizontalFOVMeasured, float screenWidth, float screenHeight)
{
	enum class FILMType
	{
		H16ML9,
		H4ML3,
	};

	constexpr FILMType mFilmType =
		(mGame == GameState::Value::Halo1) ? FILMType::H16ML9 : // confirmed
		(mGame == GameState::Value::Halo2) ? FILMType::H4ML3 :
		(mGame == GameState::Value::Halo3) ? FILMType::H16ML9 :
		(mGame == GameState::Value::Halo3ODST) ? FILMType::H16ML9 :
		(mGame == GameState::Value::HaloReach) ? FILMType::H16ML9 :
		(mGame == GameState::Value::Halo4) ? FILMType::H16ML9 :
		FILMType::H16ML9; // unpossible



	if constexpr (mGame == GameState::Value::Halo1)
	{

		float verticalFOVIn169 = 2 * std::atan(std::tan(horizontalFOVMeasured / 2) * (1080 / 1920.f));
		float actualHorizontal = 2 * std::atan(std::tan(verticalFOVIn169 / 2) * (screenWidth / screenHeight));

		return 2 * std::atan(std::tan(actualHorizontal / 2) * (screenHeight / screenWidth));
	}
	else if constexpr (mGame == GameState::Value::Halo2)
	{

		float tempValue = 2 * std::atan(std::tan(horizontalFOVMeasured / 2) * (screenHeight / screenWidth));
		return tempValue;
	}
	else
	{
		throw HCMRuntimeException("not impl yet");
	}
}



template<GameState::Value mGame>
SimpleMath::Vector3 Renderer3DImpl<mGame>::worldPointToScreenPosition(SimpleMath::Vector3 world, bool shouldClamp, float clampBorderRatio)
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



	if (shouldClamp)
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
			if (out.x < (this->screenSize.x / 2)) 	// Check if the target is on the left side of the screen
			{
				out.x = maxX; // Place it on the right (Since it's behind the player, it's the opposite)
			}
			else
			{
				out.x = minX; // Place it on the left side
			}
		}
		
		// clamp to screen
		out.x = std::clamp(out.x, minX, maxX);
		out.y = std::clamp(out.y, minY, maxY);
	}


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
	try
	{
		lockOrThrow(getGameCameraDataWeak, getGameCameraData);
		auto cameraData = getGameCameraData->getGameCameraData();
		this->cameraPosition = *cameraData.position;
		this->cameraDirection = *cameraData.lookDirForward;
		float aspectRatio = screenSizeIn.x / screenSizeIn.y; // aspect ratio is width div height!

		float verticalFov = horizontalFOVToVerticalFOV<mGame>(*cameraData.FOV, screenSizeIn.x, screenSizeIn.y);

		this->projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(verticalFov, aspectRatio, 0.001f, FAR_CLIP_3D);
		auto lookAt = this->cameraPosition + this->cameraDirection;
		this->viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(this->cameraPosition, lookAt, *cameraData.lookDirUp);
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
		runtimeExceptions->handleMessage(ex);
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