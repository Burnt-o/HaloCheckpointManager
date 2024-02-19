#pragma once
#include "pch.h"
#include "pugixml.hpp"


// represents a 3d waypoint
struct Waypoint {
	// data
	SimpleMath::Vector3 position;
	bool showDistance;
	std::optional<std::string> label;


	Waypoint(SimpleMath::Vector3 pos, bool showDis, std::optional<std::string> lab)
		: position(pos), showDistance(showDis), label(lab)
	{}



	// serialise (output to xml node)
	void serialise(pugi::xml_node wpnode) const;

	// deserialisation constructor
	Waypoint(pugi::xml_node input);

};

// a std::vector with added serialisation/deserialsation methods
struct WaypointList {
	// data
	std::vector<Waypoint> list = {};
	
#ifdef HCM_DEBUG
	WaypointList()
	{
		list = { Waypoint{{14.671300, -95.203300, -73.22693634}, true, "Hello World!"} }; // add single test waypoint at keyes start
	}
#else
	WaypointList() = default; // list starts empty
#endif


	// serialise (output to stringstream)
	friend std::ostream& operator<<(std::ostream& os, const WaypointList& dt);

	// deserialisation constructor
	WaypointList(pugi::xml_node input);

};