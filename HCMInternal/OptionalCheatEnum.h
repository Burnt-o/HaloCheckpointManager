#pragma once

// MUST HAVE SAME NAME AS ASSOCIATED CLASS (the macro will be used in a std::variant and etc to convert from enum to type)
#define ALLOPTIONALCHEATS 	\
TogglePause,\
GameTickEventHook,\
AdvanceTicks,\
ToggleBlockInput,\
ToggleFreeCursor,\
ForceCheckpoint,\
ForceRevert,\
ForceDoubleRevert,\
ForceCoreSave,\
ForceCoreLoad,\
IgnoreCheckpointChecksum,\
InjectCheckpoint,\
DumpCheckpoint,\
InjectCore,\
DumpCore,\
ForceFutureCheckpoint,\
Speedhack,\
Invulnerability,\
GetCurrentDifficulty,\
GetPlayerDatum,\
GetObjectAddress,\
GetObjectAddressCLI,\
GetCurrentLevelCode,\
GetPlayerViewAngle,\
AIFreeze,\
Medusa,\
ForceTeleport,\
ForceLaunch,\
ConsoleCommand,\
GetObjectPhysics,\
GetHavokComponent,\
GetHavokAnchorPoint,\
UnfreezeObjectPhysics,\
NaturalCheckpointDisable,\
InfiniteAmmo,\
BottomlessClip,\
DisplayPlayerInfo,\
GetAggroData,\
GetObjectHealth,\
GetTagName,\
GetObjectTagName,\
GetBipedsVehicleDatum,\
GetNextObjectDatum,\
FreeCamera,\
GetGameCameraData,\
UpdateGameCameraData,\
UserCameraInputReader,\
ThirdPersonRendering,\
BlockPlayerCharacterInput,\
GetCurrentRNG,\
EditPlayerViewAngle,\
SwitchBSP,\
GetCurrentBSP,\
DisableScreenEffects,\
HideHUD,\
OBSBypassCheck,\
SetPlayerHealth,\
Waypoint3D,\
Render3DEventProvider,\
MeasurePlayerDistanceToObject,\
SkullToggler,\
TriggerOverlay,\
GetTriggerData,\
UpdateTriggerLastChecked


enum class OptionalCheatEnum {
	ALLOPTIONALCHEATS
};

