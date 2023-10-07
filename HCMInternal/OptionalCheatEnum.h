#pragma once

// MUST HAVE SAME NAME AS ASSOCIATED CLASS (the macro will be used in a std::variant and etc to convert from enum to type)
#define ALLOPTIONALCHEATS 	\
ForceCheckpoint,\
ForceRevert,\
ForceDoubleRevert,\
ForceCoreSave,\
ForceCoreLoad,\
InjectCheckpoint,\
DumpCheckpoint,\
InjectCore,\
DumpCore,\
Speedhack,\
Invulnerability,\
GetPlayerDatum,\
GetObjectAddress\


enum class OptionalCheatEnum {
	ALLOPTIONALCHEATS
};

