#pragma once
#include "pch.h"
#include "pugixml.hpp"



// represents a 3d waypoint
struct Waypoint {
	// data
	SimpleMath::Vector3 position;
	bool showDistance;
	std::string label;
	bool enabled;


	Waypoint(SimpleMath::Vector3 pos, bool showDis, std::string lab, bool startEnabled)
		: position(pos), showDistance(showDis), label(lab), enabled(startEnabled)
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

public:
	std::vector<Waypoint> list = {}; // for the love of GOD, lock the atomic bool before reading/writing
	std::atomic_bool listInUse = false;

#ifdef HCM_DEBUG
	WaypointList()
	{
		list = { Waypoint{{14.671300, -95.203300, -73.22693634}, true, "Hello World!", true} }; // add single test waypoint at keyes start
	}
#else
	WaypointList() = default; // list starts empty
#endif


	// serialise (output to stringstream)
	friend std::ostream& operator<<(std::ostream& os, const WaypointList& dt);

	// deserialisation constructor
	WaypointList(pugi::xml_node input);

	// move constructor.. avoid using this where possible
	WaypointList(WaypointList&& other)
		: list(std::move(other.list)), listInUse((bool)other.listInUse)
	{}

};

typedef eventpp::CallbackList<void(WaypointList&)> WaypointListEvent;
typedef eventpp::CallbackList<void(Waypoint&, WaypointList&)> WaypointAndListEvent;
