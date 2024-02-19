#include "pch.h"
#include "Render2DHelpers.h"
#include "imgui.h"

namespace Render2D
{


	float scaleTextDistance(float zdistance)
	{
		// config vars
		constexpr float nearClampDistance = 0.5f; // distance at which text reaches its maximum size
		constexpr float farClampDistance = 100.f; // distance at which text reaches its minimum size
		constexpr float nearClampScale = 4.f; // the maximum scale of text when you are at nearClampDistance and closer
		constexpr float farClampScale = 0.5f; // the minimum sclae of text when you are at farClampDistance and further


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
		return remapf(zdistance, zmin, zmax, farClampScale, nearClampScale);


	}

	RECTF drawText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale)
	{
		ImFont* currentFont = ImGui::GetFont();
		currentFont->Scale = fontScale;
		ImGui::PushFont(currentFont);

		auto textSize = ImGui::CalcTextSize(textString.c_str());
		ImGui::GetBackgroundDrawList()->AddText(textPos, textColor, textString.c_str());

		ImGui::PopFont();
		return RECTF(textPos.x, textPos.y, textPos.x + textSize.x, textPos.y + textSize.y);
	}

	RECTF drawCenteredText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale)
	{
		ImFont* currentFont = ImGui::GetFont();
		currentFont->Scale = fontScale;
		ImGui::PushFont(currentFont);

		auto textSize = ImGui::CalcTextSize(textString.c_str());
		auto centeredTextPos = SimpleMath::Vector2(textPos.x - (textSize.x / 2), textPos.y - (textSize.y / 2));
		ImGui::GetBackgroundDrawList()->AddText(centeredTextPos, textColor, textString.c_str());

		ImGui::PopFont();
		return RECTF(centeredTextPos.x, centeredTextPos.y, centeredTextPos.x + textSize.x, centeredTextPos.y + textSize.y);
	}




	RECTF drawOutlinedText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale, const float& outlineSize, const uint32_t& outlineColor)
	{

		SimpleMath::Vector2 outlineTextPos;
		float outlineOffset = fontScale * outlineSize;
		auto textSize = ImGui::CalcTextSize(textString.c_str());

		for (int i = 0; i < 9; i++)
		{

			switch (i)
			{
			case 0: outlineTextPos = { textPos.x - outlineOffset, textPos.y - outlineOffset }; break;
			case 1: outlineTextPos = { textPos.x                , textPos.y - outlineOffset }; break;
			case 2: outlineTextPos = { textPos.x + outlineOffset, textPos.y - outlineOffset }; break;

			case 3: outlineTextPos = { textPos.x - outlineOffset, textPos.y + outlineOffset }; break;
			case 4: outlineTextPos = { textPos.x                , textPos.y + outlineOffset }; break;
			case 5: outlineTextPos = { textPos.x + outlineOffset, textPos.y + outlineOffset }; break;

			case 6: outlineTextPos = { textPos.x - outlineOffset, textPos.y }; break;
			case 7: outlineTextPos = { textPos.x + outlineOffset, textPos.y }; break;

			case 8: outlineTextPos = textPos;  break; // center, draws on top of all others
			}

			auto& colorToRender = i == 8 ? textColor : outlineColor;
			drawText(textString, outlineTextPos, colorToRender, fontScale);
		}

		return RECTF(textPos.x - outlineOffset, textPos.y - outlineOffset, textPos.x + textSize.x + outlineOffset, textPos.y + textSize.y + outlineOffset);

	}


	RECTF drawCenteredOutlinedText(const std::string& textString, const SimpleMath::Vector2& textPos, const uint32_t& textColor, const float& fontScale, const float& outlineSize, const uint32_t& outlineColor)
	{

		SimpleMath::Vector2 outlineTextPos;
		float outlineOffset = fontScale * outlineSize;
		auto textSize = ImGui::CalcTextSize(textString.c_str());
		auto centeredTextPos = SimpleMath::Vector2(textPos.x - (textSize.x / 2), textPos.y - (textSize.y / 2));

		for (int i = 0; i < 9; i++)
		{

			switch (i)
			{
			case 0: outlineTextPos = { centeredTextPos.x - outlineOffset, centeredTextPos.y - outlineOffset }; break;
			case 1: outlineTextPos = { centeredTextPos.x                , centeredTextPos.y - outlineOffset }; break;
			case 2: outlineTextPos = { centeredTextPos.x + outlineOffset, centeredTextPos.y - outlineOffset }; break;

			case 3: outlineTextPos = { centeredTextPos.x - outlineOffset, centeredTextPos.y + outlineOffset }; break;
			case 4: outlineTextPos = { centeredTextPos.x                , centeredTextPos.y + outlineOffset }; break;
			case 5: outlineTextPos = { centeredTextPos.x + outlineOffset, centeredTextPos.y + outlineOffset }; break;

			case 6: outlineTextPos = { centeredTextPos.x - outlineOffset, centeredTextPos.y }; break;
			case 7: outlineTextPos = { centeredTextPos.x + outlineOffset, centeredTextPos.y }; break;

			case 8: outlineTextPos = centeredTextPos;  break; // center, draws on top of all others
			}

			auto& colorToRender = i == 8 ? textColor : outlineColor;
			drawText(textString, outlineTextPos, colorToRender, fontScale);
		}

		return RECTF(centeredTextPos.x - outlineOffset, centeredTextPos.y - outlineOffset, centeredTextPos.x + textSize.x + outlineOffset, centeredTextPos.y + textSize.y + outlineOffset);

	}

	

}