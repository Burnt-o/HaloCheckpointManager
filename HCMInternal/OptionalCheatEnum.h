#pragma once

// MUST HAVE SAME NAME AS ASSOCIATED CLASS (the macro will be used in a std::variant and etc to convert from enum to type)
#define ALLOPTIONALCHEATS1 	\
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



// Turns out there's a limit on how many entries in a tuple that boost::preprocesser can handle, so split it in two!

#define ALLOPTIONALCHEATS2	\
ObjectTableRange,\
HideWatermarkCheck,\
PlayerPositionToClipboard,\
TriggerFilter,\
GetPlayerTriggerPosition,\
TrackTriggerEnterExit,\
ShieldInputPrinter,\
TriggerFilterModalDialogManager,\
GetScenarioAddress,\
GetDebugString,\
TagBlockReader,\
TagTableRange,\
GetSoftCeilingData,\
SoftCeilingOverlay,\
GetCurrentBSPSet,\
SwitchBSPSet,\
GetCurrentZoneSet,\
BSPChangeHookEvent,\
ZoneSetChangeHookEvent,\
BSPSetChangeHookEvent,\
GetActiveStructureDesignTags,\
TagReferenceReader,\
CommandConsoleManager,\
HaloScriptOutputHookEvent,\
DisableBarriers,\
GetPlayerDatumPresenter,\
FreeCameraFOVOverride,\
GetAbilityData,\
AbilityMeterOverlay,\
GetTagAddressPresenter,\
SoundClassGain,\
ChangeOOBBackground,\
DisableFog,\
SensDriftOverlay,\
RevertEventHook,\
ForceMissionRestart,\
GameEngineDetail,\
GameEngineFunctions


enum class OptionalCheatEnum {
	ALLOPTIONALCHEATS1,
	ALLOPTIONALCHEATS2,
};

