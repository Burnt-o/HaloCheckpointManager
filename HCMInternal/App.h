#pragma once
#include "Logging.h"
#include "curl\curl.h"
#include "GlobalKill.h"
#include "ModuleCache.h"
#include "ModuleHookManager.h"
#include "D3D11Hook.h"
#include "ImGuiManager.h"
#include "PointerManager.h"
#include "HCMInternalGUI.h"
#include "MessagesGUI.h"
#include "MCCStateHook.h"
#include "RPCClientInternal.h"
#include "HeartbeatTimer.h"
#include "GUIServiceInfo.h"
#include "HotkeyManager.h"
#include "HotkeyRenderer.h"
#include "HotkeyRendererImpl.h"
#include "HotkeyDefinitions.h"
#include "GetMCCVersion.h"
#include "GUIRequiredServices.h"
#include "OptionalCheatManager.h"
#include "OptionalCheatInfo.h"
#include "GUIElementStore.h"
#include "GUIElementConstructor.h"
#include "UnhandledExceptionHandler.h"
#include "SettingsSerialiser.h"

class App {


public:
	App(std::string dirPath, bool badDirPath, rpc::client& client)
	{
        // these are needed in the init exception catch block, so declared here
        auto logging = std::make_shared<Logging>();
        logging->initConsoleLogging();
        logging->SetConsoleLoggingLevel(plog::verbose);
        logging->initFileLogging(dirPath);
        logging->SetFileLoggingLevel(plog::verbose);
        PLOG_INFO << "HCMInternal initializing. DirPath: " << dirPath;
        if (badDirPath) { PLOG_ERROR << "Could not resolve HCM directory path! Files will go to MCC binary location instead of HCM directory: \n" << dirPath; }
        auto unhandled = std::make_shared<UnhandledExceptionHandler>(dirPath); PLOGV << "unhandled init";
        auto rpcClient = std::make_shared<RPCClientInternal>(client); PLOGV << "rpcClient init";
        unhandled->setRPCClient(rpcClient);
        curl_global_init(CURL_GLOBAL_DEFAULT);
        try
        {
            PLOG_DEBUG << "HCM path: " << dirPath;

            // some very important services


            ModuleCache::initialize(); PLOGV << "moduleCache init"; // static singleton still.. blah
            auto mhm = std::make_unique<ModuleHookManager>(); PLOGV << "mhm init"; // is a static singleton still.. blah 
            auto ver = std::make_shared<GetMCCVersion>(); PLOGV << "ver init";// gets the version of MCC that we're currently injected into
            auto ptr = std::make_shared<PointerManager>(ver, dirPath); PLOGV << "ptr init"; // retrieves data from github page for MultilevelPointers and other game-version-specific data


            // set up rendering
            auto d3d = std::make_shared<D3D11Hook>(); PLOGV << "d3d init"; // hooks d3d11 Present and ResizeBuffers
            auto imm = std::make_shared<ImGuiManager>(d3d, d3d->presentHookEvent); PLOGV << "imm init"; // sets up imgui context and fires off imgui render events


            auto mes = std::make_shared<MessagesGUI>(ImVec2{ 20, 20 }, imm->ForegroundRenderEvent); PLOGV << "mes init";// renders temporary messages to the screen
            auto exp = std::make_shared<RuntimeExceptionHandler>(mes); PLOGV << "exp init";// tells user if a cheat hook throws a runtime exception
            auto settings = std::make_shared<SettingsStateAndEvents>(std::make_shared<SettingsSerialiser>(dirPath, exp, mes)); PLOGV << "settings init";

            auto mccStateHook = std::make_shared<MCCStateHook>(ptr, exp); PLOGV << "gsh init";// fires event when game or level changes.

            // hotkeys
            auto hkd = std::make_shared<HotkeyDefinitions>(settings); PLOGV << "hkd init";
            auto hkm = std::make_shared<HotkeyManager>(imm->ForegroundRenderEvent, hkd, mes, dirPath); PLOGV << "hkm init"; // hotkey manager doesn't render but wants to poll inputs every frame to know if keys pressed etc
            
            auto hkrimpl = std::make_unique<HotkeyRendererImpl>(imm->ForegroundRenderEvent, mes, hkm, hkd);
            auto hkr = std::make_shared<HotkeyRenderer>(std::move(hkrimpl)); PLOGV << "hkr init"; // render hotkeys and rebinding


            // set up optional cheats and optional gui elements
            auto guireq = std::make_shared<GUIRequiredServices>(); PLOGV << "guireq init"; // defines the gui elements we want to build and which optional cheats they will require
            auto cheatfail = std::make_shared<OptionalCheatInfo>(); PLOGV << "cheatfail init"; // stores info about failed optionalCheat construction (starts empty, obviously)
            auto optionalCheats = std::make_shared<OptionalCheatManager>(guireq, cheatfail, settings, ptr, ver, mccStateHook, rpcClient, mes, exp, dirPath); PLOGV << "optionalCheats init"; // constructs and stores required optional cheats. Needs a lot of dependencies, cheats will only keep what they need.
            auto guifail = std::make_shared<GUIServiceInfo>(mes); PLOGV << "guifail init"; // stores info about gui elements that failed to construct. starts empty
            auto guistore = std::make_shared<GUIElementStore>(); PLOGV << "guistore init"; // collection starts empty, populated later by GUIElementConstructor
            auto GUICon = std::make_shared<GUIElementConstructor>(guireq, cheatfail, guistore, guifail, settings); PLOGV << "GUIMan init"; // constructs gui elements, pushing them into guistore
            guifail->printFailures();
 
            // set up main gui
            auto HCMGUI = std::make_shared<HCMInternalGUI>(mccStateHook, guistore, hkr, imm->MidgroundRenderEvent, mccStateHook->MCCStateChangedEvent); PLOGV << "HCMGUI init";// main gui. Mostly just a canvas for rendering a collection of IGUIElements that will get constructed a bit below.
            mes->setAnchorPoint(HCMGUI);



            
            auto hb = std::make_shared<HeartbeatTimer>(); PLOGV << "hb init";



            PLOG_INFO << "All services succesfully initialized! Entering main loop";
            Sleep(100);
            // Shutdown the console on successful init, at least in release mode.
            // If an initialization error occurs before this point, console will be left up so user can look at it.
            mes->addMessage("HCM successfully initialised!");
            if (badDirPath) { std::format("Could not resolve HCM directory path! Files will go to MCC binary location instead of HCM directory: \n{}", dirPath); }

#ifndef HCM_DEBUG
            PLOG_DEBUG << "Closing console";
            logging->closeConsole();
#endif

            // We live in this loop 99% of the time
            while (!GlobalKill::isKillSet()) {
                Sleep(10);
            }
            PLOG_INFO << "HCMInternal services are about to fall out of scope";
        }
        catch (HCMInitException& ex) // mandatory services that fail to init will be caught here
        {
            GlobalKill::killMe();
            std::ostringstream oss;
            oss << "\n\nHCMInternal failed initializing: " << ex.what() << std::endl
                << "Please send Burnt the log file located at: " << std::endl << logging->GetLogFileDestination();
            rpcClient->sendFatalInternalError(oss.str());
            PLOG_FATAL << oss.str();
            std::cout << "Press Enter to shutdown HCMInternal\n\n";

            std::cin.ignore();
        }
        curl_global_cleanup(); PLOG_INFO << "Curl cleaned up";
        // Auto managed resources have fallen out of scope
        PLOG_INFO << "HCMInternal services successfully shut down";


#ifdef HCM_DEBUG
        PLOG_DEBUG << "Closing console";
        logging->closeConsole();
#endif 

	}



};