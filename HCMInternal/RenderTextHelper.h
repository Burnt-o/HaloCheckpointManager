#pragma once

namespace RenderTextHelper
{
	// returns a scaling factor for fonts based on text distance,
// applying clamping and un-natural scaling to make sure text stays a readable size
	float scaleTextDistance(float zdistance);

	// RECTF return is the screen position min and max of the drawn item.
	// "Centered" variations add to textPos by the half the width and height of the the RECTF return.
	RECTF drawText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale = 16.f);
	RECTF drawCenteredText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale = 16.f);
	
	// draws text 9 times, 8 for outline and once for main.
	RECTF drawOutlinedText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale = 16.f, const float& outlinePixels = 0.5f);
	RECTF drawCenteredOutlinedText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale = 16.f, const float& outlinePixels = 0.5f);
}
 