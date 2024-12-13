#include "pch.h"
#include "ViewAngleLineList.h"



void ViewAngleLine::serialise(pugi::xml_node wpnode) const
{

	wpnode.append_child("subpixelID").text().set((uint32_t)subpixelID);
	wpnode.append_child("label").text().set(label.c_str());
	wpnode.append_child("showAngle").text().set(showAngle);
	wpnode.append_child("showSubpixelID").text().set(showSubpixelID);
	wpnode.append_child("viewAngleLineEnabled").text().set(viewAngleLineEnabled);
	wpnode.append_child("showLabel").text().set(showLabel);
	wpnode.append_child("showDistance").text().set(showDistance);
	wpnode.append_child("measureAsSubpixels").text().set(measureAsSubpixels);
	wpnode.append_child("spriteColorUseGlobal").text().set(spriteColorUseGlobal);
	wpnode.append_child("labelColorUseGlobal").text().set(labelColorUseGlobal);
	wpnode.append_child("labelScaleUseGlobal").text().set(labelScaleUseGlobal);
	wpnode.append_child("distanceColorUseGlobal").text().set(distanceColorUseGlobal);
	wpnode.append_child("distanceScaleUseGlobal").text().set(distanceScaleUseGlobal);
	wpnode.append_child("distancePrecisionUseGlobal").text().set(distancePrecisionUseGlobal);
	wpnode.append_child("spriteColor").text().set(vec4ToString(spriteColor).c_str());
	wpnode.append_child("labelColor").text().set(vec4ToString(labelColor).c_str());
	wpnode.append_child("distanceColor").text().set(vec4ToString(distanceColor).c_str());
	wpnode.append_child("labelScale").text().set(labelScale);
	wpnode.append_child("distanceScale").text().set(distanceScale);
	wpnode.append_child("distancePrecision").text().set(distancePrecision);



}

// deserialisation constructor
ViewAngleLine::ViewAngleLine(pugi::xml_node input)
{

#define deserialiseMember(memberName, asWhat, conversionFunc)\
if (input.child(###memberName)) this->memberName = conversionFunc(input.child(###memberName).text().##asWhat);


	if (input.child("subpixelID"))
	{
		auto subpixelIDInt = input.child("subpixelID").text().as_int();
		if (subpixelIDInt < 0 || subpixelIDInt > 1078530012)
		{
			subpixelIDInt = 1;
			PLOG_ERROR << "Bad deserialised subpixel ID: " << subpixelIDInt << ", " << input.child("subpixelID").text();
		}
			

		this->subpixelID = (SubpixelID)subpixelIDInt;
	}
		

	// boy I wish cpp had reflection so that this would be a one liner
	deserialiseMember(label, as_string());
	deserialiseMember(viewAngleLineEnabled, as_bool());
	deserialiseMember(showLabel, as_bool());
	deserialiseMember(showAngle, as_bool());
	deserialiseMember(showSubpixelID, as_bool());
	deserialiseMember(showDistance, as_bool());
	deserialiseMember(measureAsSubpixels, as_bool());
	deserialiseMember(labelColorUseGlobal, as_bool());
	deserialiseMember(labelScaleUseGlobal, as_bool());
	deserialiseMember(spriteColorUseGlobal, as_bool());
	deserialiseMember(distanceColorUseGlobal, as_bool());
	deserialiseMember(distanceScaleUseGlobal, as_bool());
	deserialiseMember(distancePrecisionUseGlobal, as_bool());
	deserialiseMember(spriteColor, as_string(), vec4FromString);
	deserialiseMember(labelColor, as_string(), vec4FromString);
	deserialiseMember(distanceColor, as_string(), vec4FromString);
	deserialiseMember(labelScale, as_float());
	deserialiseMember(distanceScale, as_float());
	deserialiseMember(distancePrecision, as_int());


}


// serialise (output xml to stringstream)
std::ostream& operator<<(std::ostream& os, const ViewAngleLineList& dt)
{
	pugi::xml_document doc;
	auto docRoot = doc.root();

	for (auto& wp : dt.list)
	{
		auto node = docRoot.append_child("ViewAngleLine");
		wp.serialise(node);
	}

	doc.print(os);
	return os;
}



void ViewAngleLineList::setListFromString(const std::string& str)
{
	// parse str as xml
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_string(str.c_str());
	if (result)
	{
		setListFromXML(doc);
	}
	else
	{
		throw HCMSerialisationException(std::format("error parsing ViewAngleLineList str to xml: {} @ {}", result.description(), result.offset));
	}
}


void ViewAngleLineList::setListFromXML(pugi::xml_node input)
{

	// god this is so dumb and hacky. For some reason pugi doesn't want to parse the &lt; escape chars properly
	// so we have to manually substitute it then reinterpret it as a xml node again

	std::stringstream localDocStr;
	input.print(localDocStr);

	std::string str = localDocStr.str();


	while (str.find("&lt;") != std::string::npos)
		str.replace(str.find("&lt;"), 4, "<");

	while (str.find("&gt;") != std::string::npos)
		str.replace(str.find("&gt;"), 4, ">");

	pugi::xml_document parsedLocalDoc;
	pugi::xml_parse_result result = parsedLocalDoc.load_string(str.c_str());

	if (result)
	{
		list.clear();
		// for each <ViewAngleLine> node, add it to the ViewAngleLine list using ViewAngleLine deserialisation constructor.
		for (auto waypointNode = parsedLocalDoc.first_child().first_child(); waypointNode; waypointNode = waypointNode.next_sibling())
		{
			if (strcmp(waypointNode.name(), "ViewAngleLine") != 0)
			{
				PLOG_ERROR << "bad waypointList input, child node named: " << waypointNode.name();
				std::stringstream ss;
				waypointNode.print(ss);
				PLOG_ERROR << "contents: " << std::endl << ss.str();
				continue;
			}
			list.emplace_back(ViewAngleLine(waypointNode));
		}
	}
	else
	{
		throw HCMSerialisationException(std::format("error parsing waypointList xml: {} @ {}", result.description(), result.offset));
	}

}


// deserialisation constructor
ViewAngleLineList::ViewAngleLineList(pugi::xml_node input)
{
	setListFromXML(input);
}