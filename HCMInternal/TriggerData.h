#pragma once
#include "pch.h"
#include "TriggerModel.h"
#include <bitset>

struct TriggerData 
{
	// const
	const std::string name;
	const TriggerModel model;
	const uint32_t triggerIndex;
	const bool isBSPTrigger;
	const bool isSectorType; // only relevant to ODST/Reach/H4

	// changes at run time
	bool lastCheckSuccessful = false; // updated by UpdateTriggerLastChecked
	std::optional<uint32_t> tickLastChecked = std::nullopt; // updated by UpdateTriggerLastChecked
	bool printedMessageForLastCheck = false; // set to true by TriggerOverlay, false by UpdateTriggerLastChecked. Prevents duplicate messages.
	bool playerWasInsideLastTick = false; // updated by TrackTriggerEnterExit
	SimpleMath::Vector4 currentColor;
	uint32_t currentColorU32;
	SimpleMath::Vector4 currentColorWireframe;
	void setColor(const SimpleMath::Vector4& color, const float& wireframeOpacity)
	{
		currentColor = color; // copy
		currentColorU32 = Vec4ColorToU32(currentColor); // convert

		currentColorWireframe = currentColor;
		currentColorWireframe.w += (1.f - currentColorWireframe.w) * wireframeOpacity; // boost opacity
	}

private:
	friend class TriggerDataFactory;
	explicit TriggerData(std::string triggerName, uint32_t trigIndex, bool isBSP, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up, bool isSector = false) 
		: name(triggerName), 
		model(triggerName, position, extents, forward, up),
		triggerIndex(trigIndex),
		isBSPTrigger(isBSP),
		isSectorType(isSector)
	{}
};




/*
Note for when I later implement Sectors properly:
Sector shapes are specifically called "right prisms" https://en.wikipedia.org/wiki/Prism_(geometry)
This will help me google how to do the trianglulation of the top/bottom faces
Though honestly it's just a 2-dimensional problem. Maybe find a library to do ear-clipping triangulation.
GTE has it but that shits impenetrable
Tutorialised example: https://github.com/ssell/Ear-Clipping
prob best: https://github.com/mapbox/earcut.hpp
side faces are easy to figure out.

How to do the pointContained test tho?
made doubly harder by the fact that polygons can be concave
lol maybe we could just call the games own function for "point inside trigger volume", that actually may be way easier
*/