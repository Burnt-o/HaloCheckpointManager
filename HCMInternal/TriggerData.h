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

	// bounding
	explicit TriggerData(std::string triggerName, uint32_t trigIndex, bool isBSP, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up) 
		: name(triggerName), 
		model(triggerName, position, extents, forward, up),
		triggerIndex(trigIndex),
		isBSPTrigger(isBSP),
		isSectorType(false)
	{}
	
	// sector
	explicit TriggerData(std::string triggerName, uint32_t trigIndex, bool isBSP, std::vector<SimpleMath::Vector3> sectorPointsBottom, std::vector<SimpleMath::Vector3> sectorPointsTop)
		: name(triggerName),
		model(triggerName, sectorPointsBottom, sectorPointsTop),
		triggerIndex(trigIndex),
		isBSPTrigger(isBSP),
		isSectorType(true)
	{}
};


class TriggerDataFactory
{
public:


	TriggerData makeBoundingBox(std::string triggerName, uint32_t trigIndex, bool isBSP, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
	{
		return std::move(TriggerData(triggerName, trigIndex, isBSP, position, extents, forward, up));
	}


	TriggerData makeSector(std::string triggerName, uint32_t trigIndex, bool isBSP, std::vector<SimpleMath::Vector3> sectorPointsBottom, std::vector<SimpleMath::Vector3> sectorPointsTop)
	{
		return std::move(TriggerData(triggerName, trigIndex, isBSP, sectorPointsBottom, sectorPointsTop));
	}


};