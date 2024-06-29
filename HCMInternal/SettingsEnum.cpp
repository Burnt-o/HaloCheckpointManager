#include "pch.h"
#include "SettingsEnums.h"

namespace SettingsEnums
{

	template std::ostream& operator<< (std::ostream& out, const FreeCameraObjectTrackEnum& enumValue);
	template std::ostream& operator<< (std::ostream& out, const FreeCameraObjectTrackEnumPlusAbsolute& enumValue);
	template std::ostream& operator<< (std::ostream& out, const FreeCameraInterpolationTypesEnum& enumValue);
	template std::ostream& operator<< (std::ostream& out, const TriggerInteriorStyle& enumValue);
	template std::ostream& operator<< (std::ostream& out, const TriggerRenderStyle& enumValue);
	template std::ostream& operator<< (std::ostream& out, const TriggerLabelStyle& enumValue);
	template std::ostream& operator<< (std::ostream& out, const Display2DInfoAnchorEnum& enumValue);
	template std::ostream& operator<< (std::ostream& out, const SoftCeilingRenderTypes& enumValue);
}