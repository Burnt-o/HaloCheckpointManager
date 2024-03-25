#pragma once
#include "pch.h"
#include "pugixml.hpp"



// represents a 3d waypoint
struct Waypoint {
	// data
	SimpleMath::Vector3 position = SimpleMath::Vector3{ 0, 0, 0 };
	std::string label = "unlabeled";
	bool waypointEnabled = true;
	bool showSprite = true;
	bool showLabel = true;
	bool showDistance = true;
	bool measureHorizontalOnly = false;
	bool spriteColorUseGlobal = true;
	bool spriteScaleUseGlobal = true;
	bool labelColorUseGlobal = true;
	bool labelScaleUseGlobal = true;
	bool distanceColorUseGlobal = true;
	bool distanceScaleUseGlobal = true;
	bool distancePrecisionUseGlobal = true;
	SimpleMath::Vector4 spriteColor = {1.f, 0.5f, 0.f, 1.f}; // orange
	SimpleMath::Vector4 labelColor = { 0.f, 1.f, 0.f, 1.f }; // green
	SimpleMath::Vector4 distanceColor = { 0.f, 1.f, 0.f, 1.f }; // green
	float spriteScale = 1.f;
	float labelScale = 1.f;
	float distanceScale = 1.f;
	int distancePrecision = 3;

	Waypoint(SimpleMath::Vector3 pos)
		: position(pos)
	{}



	// serialise (output to xml node)
	void serialise(pugi::xml_node wpnode) const;

	// deserialisation constructor
	Waypoint(pugi::xml_node input);

};

// a std::vector with added serialisation/deserialsation methods (and atomic bool for thread safety between ui and optionalCheats)
struct WaypointList {
	// data
private:
	void setListFromXML(pugi::xml_node input);
public:
	std::vector<Waypoint> list = {}; // for the love of GOD, lock the atomic bool before reading/writing
	std::atomic_bool listInUse = false;

#ifdef HCM_DEBUG
	WaypointList()
	{
		list = { 
			Waypoint{{14.671300, -95.203300, -73.22693634}}, // H1: test wp at keyes start
			Waypoint{{60.491402, -9.17941, 7.6300197 }}, // H2: test wp at start of cairo
		}; 

	}
#else
	WaypointList() = default; // list starts empty
#endif


	// serialise (output to stringstream)
	friend std::ostream& operator<<(std::ostream& os, const WaypointList& dt);

	// deserialisation constructor
	WaypointList(pugi::xml_node input);
	void setListFromString(const std::string& str);

	// move constructor.. avoid using this where possible
	WaypointList(WaypointList&& other)
		: list(std::move(other.list)), listInUse((bool)other.listInUse)
	{}

};

typedef eventpp::CallbackList<void(WaypointList&)> WaypointListEvent;
typedef eventpp::CallbackList<void(Waypoint&, WaypointList&)> WaypointAndListEvent;
