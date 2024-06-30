#include "pch.h"
#include "RenderTextHelper.h"
#include "directxtk\SimpleMath.h"
#include "imgui.h"
#include "ScopedImFontScaler.h"

namespace RenderTextHelper
{


	float scaleTextDistance(float zdistance)
	{
		// config vars
		constexpr float nearClampDistance = 0.5f; // distance at which text reaches its maximum size
		constexpr float farClampDistance = 100.f; // distance at which text reaches its minimum size
		constexpr float nearClampScale = 4.f; // the maximum scale of text when you are at nearClampDistance and closer
		constexpr float farClampScale = 0.75f; // the minimum sclae of text when you are at farClampDistance and further


		// clamp by world units
		zdistance = std::clamp(zdistance, nearClampDistance, farClampDistance);

		// "natural" scaling would be determined by Inverse Square law:
		// zdistance = 1.f / (zdistance * zdistance);

		// What we want is for the text to only get *slightly* smaller as it gets further away,
		// so that it remains readable.
		// To achieve this, we'll do a different scaling, then remap from the range we know it must be due to the earlier clamping

		// scaling func must have the property of "reversing" the input
		auto scalingFunc = [](float input) -> float
		{

			return 1.f / (input);

		};

		zdistance = scalingFunc(zdistance);

		// precompute input range (zmin is farClamp since scaling func will have reversed the input)
		constexpr float zmin = scalingFunc(farClampDistance);
		constexpr float zmax = scalingFunc(nearClampDistance);

		// remap
		auto remapped = remapf(zdistance, zmin, zmax, farClampScale, nearClampScale) * 2; // times two cos the base value was a lil low
		// TODO: get rid of the times two and just do a relative to screen size thing in waypoint3D

		// minimum return
		return std::max(0.01f, remapped);
					


	}


	RECTF drawText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale)
	{

		ScopedImFontScaler setFont{ fontScale };

		auto textSize = ImGui::CalcTextSize(textString.c_str());
		ImGui::GetBackgroundDrawList()->AddText(textPos, textColor, textString.c_str());

		return RECTF(textPos.x, textPos.y, textPos.x + textSize.x, textPos.y + textSize.y);
	}


	RECTF drawCenteredText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale)
	{
		ScopedImFontScaler setFont{ fontScale };

		auto textSize = ImGui::CalcTextSize(textString.c_str());
		auto centeredTextPos = SimpleMath::Vector2(textPos.x - (textSize.x / 2), textPos.y - (textSize.y / 2));
		ImGui::GetBackgroundDrawList()->AddText(centeredTextPos, textColor, textString.c_str());

		return RECTF(
			centeredTextPos.x,
			centeredTextPos.y,
			centeredTextPos.x + textSize.x,
			centeredTextPos.y + textSize.y
		);
	}




	RECTF drawOutlinedText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale, const float& outlinePixels)
	{
		ScopedImFontScaler setFont{ fontScale };

		SimpleMath::Vector2 outlineTextPos;
		SimpleMath::Vector2 textSize = ImGui::CalcTextSize(textString.c_str());
		textSize /= 8.f; // not sure why this is necessary .. only became a thing after i introduced rescalable fonts to the font scaler

		// outline color is black but with opacity of text color (opacity is MOST sig byte of uint32_t)
		const uint32_t outlineColor = textColor & 0xFF000000;

		for (int i = 0; i < 9; i++)
		{

			switch (i)
			{
			case 0: outlineTextPos = { textPos.x - outlinePixels, textPos.y - outlinePixels }; break;
			case 1: outlineTextPos = { textPos.x                , textPos.y - outlinePixels }; break;
			case 2: outlineTextPos = { textPos.x + outlinePixels, textPos.y - outlinePixels }; break;

			case 3: outlineTextPos = { textPos.x - outlinePixels, textPos.y + outlinePixels }; break;
			case 4: outlineTextPos = { textPos.x                , textPos.y + outlinePixels }; break;
			case 5: outlineTextPos = { textPos.x + outlinePixels, textPos.y + outlinePixels }; break;

			case 6: outlineTextPos = { textPos.x - outlinePixels, textPos.y }; break;
			case 7: outlineTextPos = { textPos.x + outlinePixels, textPos.y }; break;

			case 8: outlineTextPos = textPos;  break; // center, draws on top of all others
			}

			auto& colorToRender = i == 8 ? textColor : outlineColor;
			ImGui::GetBackgroundDrawList()->AddText(outlineTextPos, colorToRender, textString.c_str());

		}

		return RECTF(textPos.x - outlinePixels, textPos.y - outlinePixels, textPos.x + textSize.x + outlinePixels, textPos.y + textSize.y + outlinePixels);

	}


	RECTF drawCenteredOutlinedText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale, const float& outlinePixels)
	{
		ScopedImFontScaler setFont{ fontScale };

		SimpleMath::Vector2 outlineTextPos;
		auto textSize = ImGui::CalcTextSize(textString.c_str());
		auto centeredTextPos = SimpleMath::Vector2(textPos.x - (textSize.x / 2), textPos.y);

		// outline color is black but with opacity of text color (opacity is MOST sig byte of uint32_t)
		const uint32_t outlineColor = textColor & 0xFF000000;

		for (int i = 0; i < 9; i++)
		{

			switch (i)
			{
			case 0: outlineTextPos = { centeredTextPos.x - outlinePixels, centeredTextPos.y - outlinePixels }; break;
			case 1: outlineTextPos = { centeredTextPos.x                , centeredTextPos.y - outlinePixels }; break;
			case 2: outlineTextPos = { centeredTextPos.x + outlinePixels, centeredTextPos.y - outlinePixels }; break;

			case 3: outlineTextPos = { centeredTextPos.x - outlinePixels, centeredTextPos.y + outlinePixels }; break;
			case 4: outlineTextPos = { centeredTextPos.x                , centeredTextPos.y + outlinePixels }; break;
			case 5: outlineTextPos = { centeredTextPos.x + outlinePixels, centeredTextPos.y + outlinePixels }; break;

			case 6: outlineTextPos = { centeredTextPos.x - outlinePixels, centeredTextPos.y }; break;
			case 7: outlineTextPos = { centeredTextPos.x + outlinePixels, centeredTextPos.y }; break;

			case 8: outlineTextPos = centeredTextPos;  break; // center, draws on top of all others
			}

			auto& colorToRender = i == 8 ? textColor : outlineColor;
			ImGui::GetBackgroundDrawList()->AddText(outlineTextPos, colorToRender, textString.c_str());
		}

		// TODO: going over like 1.f outlineSize looks fucking GARBAGE, would need to add multiple layers of expansion for every 2.f of size.. sounds dumb, gotta be a better way eh. is there not some way to render the text as an image then blur it?

		return RECTF(
			centeredTextPos.x - outlinePixels, 
			centeredTextPos.y - outlinePixels, 
			centeredTextPos.x + textSize.x + outlinePixels, 
			centeredTextPos.y + textSize.y + outlinePixels
		);
	}
}