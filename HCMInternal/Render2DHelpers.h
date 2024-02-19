#pragma once
#include "pch.h"

// helper functions for imgui & directx drawing (hiding imgui/directx includes in cpp)
namespace Render2D 
{

	// returns a scaling factor for fonts based on text distance,
	// applying clamping and un-natural scaling to make sure text stays readable size
	float scaleTextDistance(float zdistance);



	// RECTF return is the screen position min and max of the drawn item.
	// "Centered" variations add to textPos by the half the width and height of the the RECTF return.

	RECTF drawText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale = 1.f);
	RECTF drawCenteredText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale = 1.f);

	// draws text 9 times, 8 for outline and once for main.
	RECTF drawOutlinedText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale, const float& outlineSize = 1.f, const uint32_t& outlineColor = 0x000000FF /*black*/);
	RECTF drawCenteredOutlinedText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale, const float& outlineSize = 1.f, const uint32_t& outlineColor = 0x000000FF /*black*/);

	// TODO: what is sprite passed as? draw with directx? 
	RECTF drawSprite();

	RECTF drawCenteredSprite();
}
