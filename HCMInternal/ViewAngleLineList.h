#pragma once
#include "pch.h"
#include "pugixml.hpp"
#include "SubpixelID.h"

/*
This is largely based on WaypointList.h/cpp
*/




// represents a view angle
struct ViewAngleLine {
	// data
	SubpixelID subpixelID;


	std::string label = "unlabeled";
	bool viewAngleLineEnabled = true;
	bool showLabel = true;
	bool showAngle = true;
	bool showSubpixelID = false;
	bool showDistance = true;
	bool measureAsSubpixels = false;
	bool labelColorUseGlobal = true;
	bool labelScaleUseGlobal = true;
	bool spriteColorUseGlobal = true;
	bool distanceColorUseGlobal = true;
	bool distanceScaleUseGlobal = true;
	bool distancePrecisionUseGlobal = true;
	SimpleMath::Vector4 spriteColor = { 1.f, 0.5f, 0.f, 1.f }; // orange
	SimpleMath::Vector4 labelColor = { 0.f, 1.f, 0.f, 1.f }; // green
	SimpleMath::Vector4 distanceColor = { 0.f, 1.f, 0.f, 1.f }; // green
	float labelScale = 1.f;
	float distanceScale = 1.f;
	int distancePrecision = 7;


	ViewAngleLine(SubpixelID subpixelID)
		: subpixelID(subpixelID)
	{}

	ViewAngleLine(SubpixelID subpixelID, std::string label)
		: subpixelID(subpixelID), label(label)
	{}

	// serialise (output to xml node)
	void serialise(pugi::xml_node wpnode) const;

	// deserialisation constructor
	ViewAngleLine(pugi::xml_node input);

};

// a std::vector with added serialisation/deserialsation methods (and atomic bool for thread safety between ui and optionalCheats)
struct ViewAngleLineList {
	// data
private:
	void setListFromXML(pugi::xml_node input);
public:
	std::vector<ViewAngleLine> list = {}; // for the love of GOD, lock the atomic bool before reading/writing
	std::atomic_bool listInUse = false;

	ViewAngleLineList(std::vector<ViewAngleLine> list) : list(list) {}; // list starts empty

#ifdef HCM_DEBUG
	ViewAngleLineList()
	{
		list = {
			ViewAngleLine{SubpixelID::fromFloat(0.f)}, // zero threshold. should be in positive x direction.
			ViewAngleLine{SubpixelID::fromFloat(1.58f)} // facing leftwards
		};

	}
#else

	ViewAngleLineList()
#endif


	// serialise (output to stringstream)
	friend std::ostream& operator<<(std::ostream& os, const ViewAngleLineList& dt);

	// deserialisation constructor
	ViewAngleLineList(pugi::xml_node input);
	void setListFromString(const std::string& str);

	// move constructor.. avoid using this where possible
	ViewAngleLineList(ViewAngleLineList&& other)
		: list(std::move(other.list)), listInUse((bool)other.listInUse)
	{}

};

typedef eventpp::CallbackList<void(ViewAngleLineList&)> ViewAngleLineListEvent;
typedef eventpp::CallbackList<void(ViewAngleLine&, ViewAngleLineList&)> ViewAngleLineAndListEvent;
