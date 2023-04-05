
#include "includes.h"
#include "renderer.h"
//#include "hookd3d.h"
#include "draw-text.h"
#include <iostream>
#include "hookd3d.h"


const std::wstring fontFamily = L"Courier New";

bool debugme = true;

void DrawMessageText()
{
	// Need to normalise screen position
	//Vec2 normPos = Vec2(pos.x / g_screenWidth, pos.y / g_screenHeight);
	Vec2 normPos = Vec2(10.0f, 10.0f);
	if (debugme)
	{
		debugme = false;
		std::cout << "normPosx: " << normPos.x << ", normPosy: " << normPos.y << std::endl;
	}

	// Add a black outline to the text to make it a bit more readable
	if (outlineWidth > 0 && outlineStrength >= 0 && outlineStrength <= 1)
	{
		Color outlineColor = DirectX::Colors::Black; // We want the outline to be black
		outlineColor.f[3] = color.f[3] * outlineStrength; // but with input colors Alpha, multiplied by outlineStrength

		// Simply draw the text a bit (outlineWidth number of pixels) to each side of where the final text will go
		renderer->drawText(Vec2(normPos.x + outlineWidth, normPos.y), text, outlineColor, fontSize);
		renderer->drawText(Vec2(normPos.x - outlineWidth, normPos.y), text, outlineColor, fontSize);
		renderer->drawText(Vec2(normPos.x, normPos.y + outlineWidth), text, outlineColor, fontSize);
		renderer->drawText(Vec2(normPos.x, normPos.y - outlineWidth), text, outlineColor, fontSize);

	}

	// Draw the regular text over the top of the black outline
	renderer->drawText(Vec2(normPos.x, normPos.y), text, color, fontSize);
}

