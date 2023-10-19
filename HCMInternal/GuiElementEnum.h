#pragma once
#include "pch.h"
#include <boost\preprocessor.hpp>

//#define ALL_SUPPORTED_GAMES (Halo1)(Halo2)(Halo3)(Halo3ODST)(HaloReach)(Halo4)
//
//#define ALLGUIELEMENTS 	\
//((controlHeadingGUI)ALL_SUPPORTED_GAMES)\
//((saveManagementHeadingGUI)ALL_SUPPORTED_GAMES)\
//	((forceCheckpointGUI)ALL_SUPPORTED_GAMES)\
//	((forceRevertGUI)ALL_SUPPORTED_GAMES)\
//	((forceDoubleRevertGUI)(Halo2)(Halo3)(Halo3ODST)(HaloReach)(Halo4))\
//	((forceCoreSaveGUI)(Halo1))\
//	((forceCoreLoadGUI)(Halo1))\
//	((injectCheckpointGUI)ALL_SUPPORTED_GAMES)\
//	((dumpCheckpointGUI)ALL_SUPPORTED_GAMES)\
//	((injectCoreGUI)(Halo1))\
//	((dumpCoreGUI)(Halo1))\
//((cheatsHeadingGUI)ALL_SUPPORTED_GAMES)\
//	((speedhackGUI)ALL_SUPPORTED_GAMES)\
//	((invulnGUI)ALL_SUPPORTED_GAMES)\
//	((invulnNPCGUI)ALL_SUPPORTED_GAMES)\
//	((aiFreezeGUI)ALL_SUPPORTED_GAMES) \
//	((consoleCommandGUI)(Halo1)) \
//((overlaysHeadingGUI)ALL_SUPPORTED_GAMES)\
//((cameraHeadingGUI)ALL_SUPPORTED_GAMES)\
//((theaterHeadingGUI)(Halo3)(Halo3ODST)(HaloReach)(Halo4))\
//


#define ALL_SUPPORTED_GAMES Halo1, Halo2, Halo3, Halo3ODST, HaloReach, Halo4
#define ALL_GAMES_AND_MAINMENU Halo1, Halo2, Halo3, Halo3ODST, HaloReach, Halo4, NoGame

// A sequence of pairs, where the first element of a pair is the GUIElementEnum name, and the second element is a tuple of supported games for that guielement
#define ALLGUIELEMENTS_ANDSUPPORTEDGAMES \
((controlHeadingGUI, (ALL_GAMES_AND_MAINMENU)))\
	((toggleGUIHotkeyGUI, (ALL_GAMES_AND_MAINMENU)))\
	((GUISettingsSubheading, (ALL_GAMES_AND_MAINMENU)))\
		((GUIShowingFreesCursor, (ALL_GAMES_AND_MAINMENU)))\
		((GUIShowingBlocksInput, (ALL_GAMES_AND_MAINMENU)))\
		((GUIShowingPausesGame, (ALL_GAMES_AND_MAINMENU)))\
	((togglePauseGUI, (ALL_GAMES_AND_MAINMENU)))\
	((togglePauseSettingsSubheading, (ALL_GAMES_AND_MAINMENU)))\
		((pauseAlsoFreesCursorGUI, (ALL_GAMES_AND_MAINMENU)))\
		((pauseAlsoBlocksInputGUI, (ALL_GAMES_AND_MAINMENU)))\
	((showGUIFailuresGUI, (ALL_GAMES_AND_MAINMENU)))\
((saveManagementHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((forceCheckpointGUI, (ALL_SUPPORTED_GAMES)))\
	((forceRevertGUI, (ALL_SUPPORTED_GAMES)))\
	((forceDoubleRevertGUI, (Halo2, Halo3, Halo3ODST, HaloReach, Halo4)))\
	((forceCoreSaveGUI, (Halo1)))\
	((forceCoreLoadGUI, (Halo1)))\
	((injectCheckpointGUI, (ALL_SUPPORTED_GAMES)))\
	((injectCheckpointSettingsSubheading, (ALL_SUPPORTED_GAMES)))\
		((injectCheckpointLevelCheck, (ALL_SUPPORTED_GAMES)))\
	((dumpCheckpointGUI, (ALL_SUPPORTED_GAMES)))\
	((dumpCheckpointSettingsSubheading, (ALL_SUPPORTED_GAMES)))\
		((dumpCheckpointAutonameGUI, (ALL_SUPPORTED_GAMES)))\
	((injectCoreGUI, (Halo1)))\
	((injectCoreSettingsSubheading, (Halo1)))\
		((injectCoreLevelCheck, (Halo1)))\
	((dumpCoreSettingsSubheading, (Halo1)))\
		((dumpCoreAutonameGUI, (Halo1)))\
	((dumpCoreGUI, (Halo1)))\
((cheatsHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((speedhackGUI, (ALL_SUPPORTED_GAMES)))\
	((invulnGUI, (ALL_SUPPORTED_GAMES)))\
	((invulnNPCGUI, (ALL_SUPPORTED_GAMES)))\
	((aiFreezeGUI, (ALL_SUPPORTED_GAMES)))\
	((consoleCommandGUI, (Halo1)))\
((overlaysHeadingGUI, (ALL_SUPPORTED_GAMES)))\
((cameraHeadingGUI, (ALL_SUPPORTED_GAMES)))\
((theaterHeadingGUI, (Halo3,Halo3ODST,HaloReach,Halo4)))








#define MAKE_FIRSTOFPAIR_SET(r, d, seq) BOOST_PP_TUPLE_ELEM(0, seq),
#define MAKE_ALL_FIRSTOFPAIR(seq) BOOST_PP_SEQ_FOR_EACH(MAKE_FIRSTOFPAIR_SET, _, seq)
#define ALLGUIELEMENTS MAKE_ALL_FIRSTOFPAIR(ALLGUIELEMENTS_ANDSUPPORTEDGAMES)


enum class GUIElementEnum {
	ALLGUIELEMENTS
};
