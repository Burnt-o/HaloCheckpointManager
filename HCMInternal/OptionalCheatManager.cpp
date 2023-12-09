#include "pch.h"
#include "OptionalCheatManager.h"
#include "OptionalCheatEnum.h"
#include "IOptionalCheat.h"

#include "TogglePause.h"
#include "ToggleFreeCursor.h"
#include "ToggleBlockInput.h"

#include "ForceCheckpoint.h"
#include "ForceRevert.h"
#include "ForceDoubleRevert.h"

#include "InjectCheckpoint.h"
#include "InjectCore.h"
#include "DumpCheckpoint.h"
#include "DumpCore.h"
#include "ForceCoreSave.h"
#include "ForceCoreLoad.h"
#include "Speedhack.h"
#include "Invulnerability.h"
#include "GetPlayerDatum.h"
#include "GetObjectAddress.h"
#include "AIFreeze.h"
#include "ConsoleCommand.h"
#include "GetCurrentDifficulty.h"
#include "IgnoreCheckpointChecksum.h"
#include "AdvanceTicks.h"
#include "GameTickEventHook.h"
#include "ForceTeleport.h"
#include "ForceLaunch.h"
#include "GetPlayerViewAngle.h"
#include "GetObjectPhysics.h"
#include "GetHavokComponent.h"
#include "GetHavokAnchorPoint.h"
#include "UnfreezeObjectPhysics.h"
#include "GetObjectAddressCLI.h"
#include "Medusa.h"
#include "NaturalCheckpointDisable.h"
#include "InfiniteAmmo.h"
#include "BottomlessClip.h"
#include "DisplayPlayerInfo.h"
#include "GetObjectHealth.h"
#include "GetAggroData.h"
#include "GetTagName.h"
#include "GetObjectTagName.h"
#include "GetBipedsVehicleDatum.h"
#include "GetNextObjectDatum.h"
#include "FreeCamera.h"
#include "GetGameCameraData.h"
#include "UpdateGameCameraData.h"
#include "UserCameraInputReader.h"
#include "ThirdPersonRendering.h"
#include "BlockPlayerCharacterInput.h"
#include "GetCurrentRNG.h"
#include "EditPlayerViewAngle.h"
#include "SwitchBSP.h"
#include "GetCurrentBSP.h"
#include "DisableScreenEffects.h"

#include "DIContainer.h"
#include "map.h"
#include <boost\preprocessor.hpp>
#include "DirPathContainer.h"
#include "IMakeOrGetCheat.h"




typedef std::map<std::pair<GameState, OptionalCheatEnum>, std::shared_ptr<IOptionalCheat>> CheatCollection;

class OptionalCheatManager::OptionalCheatStore 
{
public:

private:

	// cheats that depend on other cheats will need to access this to check if they're already initialised before attempting to init it themselves.
	// This is the main point of ownership keeping all the optional cheats alive. 
	std::shared_ptr<CheatCollection> cheatCollection = std::make_shared<CheatCollection>();
	DIContainer<IMakeOrGetCheat, SettingsStateAndEvents, PointerManager, IGetMCCVersion, IMCCStateHook, ISharedMemory, IMessagesGUI, RuntimeExceptionHandler, DirPathContainer, IModalDialogRenderer, ControlServiceContainer, RenderEvent> dicon;


public:
	OptionalCheatStore(std::shared_ptr<IMakeOrGetCheat> cheatConstructor, 
		std::shared_ptr<SettingsStateAndEvents> settings,
		std::shared_ptr<PointerManager> ptr,
		std::shared_ptr<IGetMCCVersion> ver,
		std::shared_ptr<IMCCStateHook> mccStateHook,
		std::shared_ptr<ISharedMemory> sharedMem,
		std::shared_ptr<IMessagesGUI> mes,
		std::shared_ptr<RuntimeExceptionHandler> exp,
		std::string dirPath,
		std::shared_ptr<IModalDialogRenderer> modal,
		std::shared_ptr<ControlServiceContainer> control,
		std::shared_ptr<RenderEvent> overlayRenderEvent)
		:
		// create a di container with the dependencies that the cheats will need
		// remember: you need to register types as the base interface the optionalCheats will want to resolve
		dicon(cheatConstructor, settings, ptr, ver, mccStateHook, sharedMem, mes, exp, std::make_shared<DirPathContainer>(dirPath), modal, control, overlayRenderEvent)
	{ 
		
	}

	~OptionalCheatStore()
	{
		PLOG_DEBUG << "~OptionalCheatStore";
		// important: the cheat collection must be destroyed before the dicon, so the cheat destructors are still guarenteed access to their services
		cheatCollection->clear();
	}

	friend class OptionalCheatConstructor;

};

class OptionalCheatConstructor : public IMakeOrGetCheat
{

private:
	// weak reference to the stores cheatCollection while we construct cheats
	std::shared_ptr<CheatCollection> cheatCollection;

public:
	OptionalCheatConstructor()
	{

	}

	// needs to be split away from constructor so we can use pass a weak ptr to OptionalCheatStore
	void createCheats(std::weak_ptr<OptionalCheatManager::OptionalCheatStore> cheatStore, std::shared_ptr<IGUIRequiredServices> reqSer, std::shared_ptr<OptionalCheatInfo> info)
	{
		if (cheatStore.expired()) throw HCMInitException("Bad cheatStore weak ptr");
		cheatCollection = cheatStore.lock()->cheatCollection;

		PLOG_DEBUG << "Looping over required services";
		// loop over each cheat-game combo in requiredServices, pushing them into our cheatCollection as we make them (or telling info about it if failed construction)
		for (const std::pair<GameState, OptionalCheatEnum>& gameCheatPair : reqSer->getAllRequiredServices())
		{
			try
			{
				// create the cheat 
				getOrMakeCheat(gameCheatPair, cheatStore.lock()->dicon);
				info->setInfo(gameCheatPair, {});
			}
			catch (HCMInitException ex)
			{
				// update OptionalCheatInfoManager with failure. GUIElementManager will check this later to know what GUIElements will work or not
				info->setInfo(gameCheatPair, { ex });
			}
			catch (std::bad_weak_ptr ex)
			{
				HCMInitException converted(std::format("std::bad_weak_ptr exception! {}", ex.what()));
				info->setInfo(gameCheatPair, { converted });
			}
		}

		cheatCollection.reset();

	}

	// Very likely to throw HCMInitExceptions so be ready to catch them.
	// called in OptionalCheatConstructor createCheats for loop, AND thru dicon by cheats that depend on other cheats.
	std::shared_ptr< IOptionalCheat> getOrMakeCheat(const std::pair<GameState, OptionalCheatEnum>& gameCheatPair, IDIContainer& dicon);


};



OptionalCheatManager::OptionalCheatManager(std::shared_ptr<IGUIRequiredServices> reqSer, std::shared_ptr<OptionalCheatInfo> info,
	/* rest is cheat construction stuff that will get stuffed into a DIContainer later*/
	std::shared_ptr<SettingsStateAndEvents> settings,
	std::shared_ptr<PointerManager> ptr, 
	std::shared_ptr<IGetMCCVersion> ver, 
	std::shared_ptr<IMCCStateHook> mccStateHook, 
	std::shared_ptr<ISharedMemory> sharedMem, 
	std::shared_ptr<IMessagesGUI> mes, 
	std::shared_ptr<RuntimeExceptionHandler> exp, 
	std::string dirPath, 
	std::shared_ptr<IModalDialogRenderer> modal,
	std::shared_ptr<ControlServiceContainer> control,
	std::shared_ptr<RenderEvent> overlayRenderEvent)
	: constructorPimpl(std::make_shared<OptionalCheatConstructor>())

{
	storePimpl = std::make_shared<OptionalCheatStore>(constructorPimpl, settings, ptr, ver, mccStateHook, sharedMem, mes, exp, dirPath, modal, control, overlayRenderEvent);

	// Ah yes a cyclic dependency? But actually the storePimpl will go in as a weak ptr, and it doesn't keep it after the method finishes anyway
	constructorPimpl->createCheats(storePimpl, reqSer, info);
}


OptionalCheatManager::~OptionalCheatManager() = default;


std::shared_ptr< IOptionalCheat> OptionalCheatConstructor::getOrMakeCheat(const std::pair<GameState, OptionalCheatEnum>& gameCheatPair, IDIContainer& dicon)
{
	PLOG_VERBOSE << "Creating cheat: " << gameCheatPair.first << "::" << magic_enum::enum_name(gameCheatPair.second);
	//std::map<std::pair<GameState, OptionalCheatEnum>, std::shared_ptr<IOptionalCheat>>

	// Macro shennanigans to make a switch case for every OptionalCheatEnum to create associated class.
	// This macro is why it's crucial the OptionalCheatEnum shares the exact same name as the associated class.
#define _PPSTUFF_MAKECASE1(_var)																\
case OptionalCheatEnum::_var:																	\
	if(!cheatCollection->contains(gameCheatPair))												\
	{																							\
	std::shared_ptr<IOptionalCheat> cheat = std::make_shared<_var>(gameCheatPair.first, dicon); \
	cheatCollection->insert(std::make_pair(gameCheatPair, cheat));									\
	}																							\
return cheatCollection->at(gameCheatPair);


#define _PPSTUFF_MAKECASE2(r, d, _var)  _PPSTUFF_MAKECASE1(_var) 
#define _PPSTUFF_MAKECASE_SEQ(vseq) _PPSTUFF_MAKECASE1(BOOST_PP_SEQ_HEAD(vseq)) \
        BOOST_PP_SEQ_FOR_EACH(_PPSTUFF_MAKECASE2,,BOOST_PP_SEQ_TAIL(vseq)) 
#define MAKECASE(...) _PPSTUFF_MAKECASE_SEQ(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

	switch (gameCheatPair.second)
	{
		MAKECASE(ALLOPTIONALCHEATS);
		// evals to
		// 
		// case OptionalCheatEnum::ForceCheckpoint: 
		// if(!cheatCollection->contains(gameCheatPair))
		// { 
		// std::shared_ptr<IOptionalCheat> cheat = std::make_shared<ForceCheckpoint>(gameCheatPair.first, dicon);
		// cheatCollection->at(gameCheatPair) = cheat;
		// }
		// return cheatCollection->at(gameCheatPair);
		// 
		// .. etc
	default:
		throw HCMInitException("getOrMakeCheat recieved invalid OptionalCheatEnum (did you forget to add an identically named class for your enum entry?)");
	}
}
