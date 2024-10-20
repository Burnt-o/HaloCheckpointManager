#pragma once
#include <type_traits>

namespace SettingsEnums
{ 
	enum class FreeCameraObjectTrackEnum
	{
		Player,
		CustomObject
	};

	enum class FreeCameraObjectTrackEnumPlusAbsolute
	{
		Player,
		CustomObject,
		ManualPosition
	};

	enum class FreeCameraInterpolationTypesEnum
	{
		None,
		Linear
	};

	enum class TriggerInteriorStyle { Normal, Patterned, DontRender };

	enum class TriggerRenderStyle { Solid, Wireframe, SolidAndWireframe, None };

	enum class TriggerLabelStyle { None, Center, Corner };


	enum class SoftCeilingRenderTypes { Bipeds, Vehicles, BipedsOrVehicles };

	enum class SoftCeilingRenderDirection { Front, Back, Both };

	enum class ScreenAnchorEnum
	{
		TopLeft,
		TopRight,
		BottomRight,
		BottomLeft
	};




	template<class T, typename = std::enable_if_t<std::is_enum_v<T>>>
	inline std::ostream& operator<< (std::ostream& out, const T& enumValue)
	{
		return out << (int)enumValue;
	}
}