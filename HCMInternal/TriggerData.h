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