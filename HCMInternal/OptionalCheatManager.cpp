#include "pch.h"
#include "OptionalCheatManager.h"
#include "OptionalCheatEnum.h"
#include "IOptionalCheat.h"

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

#include "DIContainer.h"
#include "map.h"
#include <boost\preprocessor.hpp>
#include "DirPathContainer.h"
#include "IMakeOrGetCheat.h"





class OptionalCheatManager::OptionalCheatManagerImpl : public std::enable_shared_from_this<OptionalCheatManagerImpl>, public IMakeOrGetCheat
{
private:

	// cheats that depend on other cheats will need to access this to check if they're already initialised before attempting to init it themselves
	typedef std::map<std::pair<GameState, OptionalCheatEnum>, std::shared_ptr<IOptionalCheat>> CheatCollection;
	CheatCollection cheatCollection;


public:
	OptionalCheatManagerImpl()
	{ 
	}

	// needs to be split away from constructor so we can use shared_from_this
	void createCheats(std::shared_ptr<IGUIRequiredServices> reqSer, std::shared_ptr<OptionalCheatInfo> info,
		/* rest is cheat construction stuff that will get stuffed into a DIContainer later*/
		std::shared_ptr<SettingsStateAndEvents> settings, std::shared_ptr<PointerManager> ptr, std::shared_ptr<IGetMCCVersion> ver, std::shared_ptr<MCCStateHook> mccStateHook, std::shared_ptr<SharedMemoryInternal> sharedMem, std::shared_ptr<MessagesGUI> mes, std::shared_ptr<RuntimeExceptionHandler> exp, std::string dirPath)
	{
		// create a di container with the dependencies that the cheats will need
		// remember: you need to register types as the base interface the optionalCheats will want to resolve
		DIContainer<IMakeOrGetCheat, SettingsStateAndEvents, PointerManager, IGetMCCVersion, MCCStateHook, SharedMemoryInternal, MessagesGUI, RuntimeExceptionHandler, DirPathContainer> dicon
		{shared_from_this(), settings, ptr, ver, mccStateHook, sharedMem, mes, exp,std::make_shared<DirPathContainer>(dirPath)};

		// loop over each cheat-game combo in requiredServices, pushing them into our cheatCollection as we make them (or telling info about it if failed construction)
		for (const std::pair<GameState, OptionalCheatEnum>& gameCheatPair : reqSer->getAllRequiredServices())
		{
			try
			{
				// create the cheat 
				getOrMakeCheat(gameCheatPair, dicon);
				info->setInfo(gameCheatPair, {});
			}
			catch (HCMInitException ex)
			{
				// update OptionalCheatInfoManager with failure. GUIElementManager will check this later to know what GUIElements will work or not
				info->setInfo(gameCheatPair, { ex });
			}
		}
	}

	// Very likely to throw HCMInitExceptions so be ready to catch them.
	// called in OptionalCheatManagerImpl constructors for loop, AND thru dicon by cheats that depend on other cheats.
	std::shared_ptr< IOptionalCheat> getOrMakeCheat(const std::pair<GameState, OptionalCheatEnum>& gameCheatPair, IDIContainer& dicon); 


};



OptionalCheatManager::OptionalCheatManager(std::shared_ptr<IGUIRequiredServices> reqSer, std::shared_ptr<OptionalCheatInfo> info,
	/* rest is cheat construction stuff that will get stuffed into a DIContainer later*/
	std::shared_ptr<SettingsStateAndEvents> settings, std::shared_ptr<PointerManager> ptr, std::shared_ptr<IGetMCCVersion> ver, std::shared_ptr<MCCStateHook> mccStateHook, std::shared_ptr<SharedMemoryInternal> sharedMem, std::shared_ptr<MessagesGUI> mes, std::shared_ptr<RuntimeExceptionHandler> exp, std::string dirPath)
	: pimpl(std::make_shared<OptionalCheatManagerImpl>()) 

{
	pimpl->createCheats(reqSer, info, settings, ptr, ver, mccStateHook, sharedMem, mes, exp, dirPath);
}


OptionalCheatManager::~OptionalCheatManager() = default;



std::shared_ptr< IOptionalCheat> OptionalCheatManager::OptionalCheatManagerImpl::getOrMakeCheat(const std::pair<GameState, OptionalCheatEnum>& gameCheatPair, IDIContainer& dicon)
{
	// macro shennanigans to make a switch case for every OptionalCheatEnum to create associated class
#define _PPSTUFF_MAKECASE1(_var) case OptionalCheatEnum::_var: \
	if(!cheatCollection.contains(gameCheatPair)) \
	{ \
	auto cheat = std::make_shared<_var>(gameCheatPair.first, dicon);\
	cheatCollection.emplace(gameCheatPair, cheat);\
	} \
return cheatCollection.at(gameCheatPair);

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
		// if(!cheatCollection.contains(gameCheatPair))
		// { 
		// auto cheat = std::make_shared<ForceCheckpoint>(gameCheatPair.first, dicon);
		// cheatCollection.emplace(gameCheatPair, cheat);
		// }
		// return cheatCollection.at(gameCheatPair);
		// 
		// .. etc
	default:
		throw HCMInitException("makeswitchcheat recieved invalid optionalCheatEnum");
	}
}