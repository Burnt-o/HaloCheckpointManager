#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"



template<GameState::Value mGame>
bool Renderer3DImpl<mGame>::pointBehindCamera(const SimpleMath::Vector3& point)
{
	return (point - this->cameraPosition).Dot(this->cameraDirection) < 0;
}

void clampScreenPosition(SimpleMath::Vector3& screenPosition, const SimpleMath::Vector2& screenCenter, bool reverse = false)
{
	// Need to align unclampedScreenPos to center of screen
	screenPosition.x -= screenCenter.x;
	screenPosition.y -= screenCenter.y;

	// find angle between centre of screen and the unclamped screen pos.
	float screenPosAngle = std::atan2f(screenPosition.y, screenPosition.x);

	if (reverse)
		screenPosAngle = std::fmodf(screenPosAngle + DirectX::XM_2PI, DirectX::XM_2PI) - DirectX::XM_PI;

	float screenPosSlope = std::tanf(screenPosAngle);



	// line equation stuff. 
	if (screenPosition.x > 0)
	{
		// screenPosition is off the the right of the screen. New screen position needs to be at x-max., with y position y = mx.
		screenPosition = SimpleMath::Vector3(screenCenter.x, screenCenter.x * screenPosSlope, screenPosition.z);
	}
	else
	{
		screenPosition = SimpleMath::Vector3(-screenCenter.x, -screenCenter.x * screenPosSlope, screenPosition.z);
	}

	if (screenPosition.y > screenCenter.y)
	{
		screenPosition = SimpleMath::Vector3(screenCenter.y / screenPosSlope, screenCenter.y, screenPosition.z);
	}
	else if (screenPosition.y < -screenCenter.y) // might need to times screenCenter.y by two.
	{
		screenPosition = SimpleMath::Vector3(-screenCenter.y / screenPosSlope, -screenCenter.y, screenPosition.z);
	}

	// bring screenPos back to it's original reference 
	screenPosition.x += screenCenter.x;
	screenPosition.y += screenCenter.y;

	DEBUG_KEY_LOG('8', PLOG_DEBUG << "post clamp screen pos: " << screenPosition; );


}


float measurePlanePointDistance(const SimpleMath::Plane& plane, const SimpleMath::Vector3& point)
{
	return plane.DotNormal((point - plane));
}



template<GameState::Value mGame>
bool Renderer3DImpl<mGame>::pointOnScreen(const SimpleMath::Vector3& worldPointPosition)
{
	return this->frustumViewWorld.Contains(worldPointPosition) != DirectX::ContainmentType::DISJOINT;
}




template<GameState::Value mGame>
SimpleMath::Vector3 Renderer3DImpl<mGame>::worldPointToScreenPosition(SimpleMath::Vector3 worldPointPosition, bool shouldFlipBehind)
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

	if (pointBehindCamera(worldPointPosition))
	{
		// points in semisphere behind camera need to have their clipSpace mirrored back the other direction
			clipSpace.x *= -1.f;
			clipSpace.y *= -1.f;

	}
	
	// convert from clipSpace (-1..+1) to screenSpace (0..Width, 0..Height)
	auto out = SimpleMath::Vector3
	(
		(1.f + clipSpace.x) * 0.5f * this->screenSize.x,
		(1.f - clipSpace.y) * 0.5f * this->screenSize.y,
		clipSpace.z
	);

	return out;
}




template<GameState::Value mGame>
SimpleMath::Vector3 Renderer3DImpl<mGame>::worldPointToScreenPositionClamped(SimpleMath::Vector3 worldPointPosition , int screenEdgeOffset , bool* appliedClamp)
{
https://github.com/jinincarnate/off-screen-indicator/blob/master/Off%20Screen%20Indicator/Assets/Scripts/OffScreenIndicatorCore.cs

	auto screenPosition = worldPointToScreenPosition(worldPointPosition, true);

	DEBUG_KEY_LOG('8', PLOG_DEBUG << "pre  clamp screen pos: " << screenPosition; );

	// if point is on screen it doesn't need clamping
	if (pointOnScreen(worldPointPosition))
	{
		if (appliedClamp) *appliedClamp = false;
		return screenPosition;
	}
	// else, need to clamp.
	if (appliedClamp) *appliedClamp = true;

	clampScreenPosition(screenPosition, this->screenCenter);
	return screenPosition;

}

template<GameState::Value mGame>
float Renderer3DImpl<mGame>::cameraDistanceToWorldPoint(SimpleMath::Vector3 worldPointPosition)
{
	return SimpleMath::Vector3::Distance(this->cameraPosition, worldPointPosition);
}





// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;