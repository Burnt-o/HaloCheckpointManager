#pragma once

namespace PointerDataGetter
{
	constexpr std::string_view githubPath = "https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/master/HCMInternal/InternalPointerData.xml";
	std::string getXMLDocument(std::string HCMdirPath);
}