#include "pch.h"
#include "WaypointList.h"



void Waypoint::serialise(pugi::xml_node wpnode) const
{

	auto posNode = wpnode.append_child("position");
	posNode.append_child("x").text().set(position.x);
	posNode.append_child("y").text().set(position.y);
	posNode.append_child("z").text().set(position.z);
	wpnode.append_child("label").text().set(label.c_str());
	wpnode.append_child("waypointEnabled").text().set(waypointEnabled);
	wpnode.append_child("showSprite").text().set(showSprite);
	wpnode.append_child("showLabel").text().set(showLabel);
	wpnode.append_child("showDistance").text().set(showDistance);
	wpnode.append_child("spriteColorUseGlobal").text().set(spriteColorUseGlobal);
	wpnode.append_child("spriteScaleUseGlobal").text().set(spriteScaleUseGlobal);
	wpnode.append_child("labelColorUseGlobal").text().set(labelColorUseGlobal);
	wpnode.append_child("labelScaleUseGlobal").text().set(labelScaleUseGlobal);
	wpnode.append_child("distanceColorUseGlobal").text().set(distanceColorUseGlobal);
	wpnode.append_child("distanceScaleUseGlobal").text().set(distanceScaleUseGlobal);
	wpnode.append_child("waypointEnabled").text().set(distancePrecisionUseGlobal);
	wpnode.append_child("spriteColor").text().set(vec4ToString(spriteColor).c_str());
	wpnode.append_child("labelColor").text().set(vec4ToString(labelColor).c_str());
	wpnode.append_child("distanceColor").text().set(vec4ToString(distanceColor).c_str());
	wpnode.append_child("spriteScale").text().set(spriteScale);
	wpnode.append_child("labelScale").text().set(labelScale);
	wpnode.append_child("distanceScale").text().set(distanceScale);
	wpnode.append_child("distancePrecision").text().set(distancePrecision);



}

// deserialisation constructor
Waypoint::Waypoint(pugi::xml_node input)
{
#ifdef HCM_DEBUG
	throw E_NOTIMPL;
#else
	static_assert(false, "IMPLEMENT WAYPOINT (DE)SERIALISATION");
#endif
}


// serialise (output xml to stringstream)
std::ostream& operator<<(std::ostream& os, const WaypointList& dt)
{
	pugi::xml_document doc;

	for (auto& wp : dt.list)
	{
		auto node = doc.append_child("waypoint");
		wp.serialise(node);
	}

	doc.print(os);
	return os;
}




	// deserialisation constructor
WaypointList::WaypointList(pugi::xml_node input)
{
#ifdef HCM_DEBUG
	throw E_NOTIMPL;
#else
	static_assert(false, "IMPLEMENT WAYPOINT (DE)SERIALISATION");
#endif
}