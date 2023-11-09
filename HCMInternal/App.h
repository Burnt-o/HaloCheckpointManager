#pragma once
#include "Logging.h"
#include "GlobalKill.h"
#include "ModuleCache.h"
#include "ModuleHookManager.h"
#include "D3D11Hook.h"
#include "ImGuiManager.h"
#include "PointerManager.h"
#include "HCMInternalGUI.h"
#include "MessagesGUI.h"
#include "MCCStateHook.h"
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
#include "SharedMemoryInternal.h"
#include "DynamicStructFactory.h"
#include "IMessagesGUI.h"
#include "HotkeyEventsLambdas.h"
#include "ModalDialogRenderer.h"
#include "ControlServiceContainer.h"


class App {


public:
	App()
	{
        std::shared_ptr<UnhandledExceptionHandler> unhandled; // init later, but we need it to be the last thing to go out of scope

        // these are needed in the init exception catch block, so declared here
        auto logging = std::make_shared<Logging>();

#ifdef HCM_DEBUG
        logging->initConsoleLogging();
        logging->SetConsoleLoggingLevel(plog::verbose);
#endif
        std::shared_ptr<SharedMemoryInternal> sharedMem;
        
        try
        {
            sharedMem = std::make_shared<SharedMemoryInternal>();
        }
        catch(HCMInitException ex)
        {
            int msgboxID = MessageBoxA(
                NULL,
                std::format("HCM internal failed to create shared memory, error:\n{}", ex.what()).c_str(),
                "Halo checkpoint manager error",
                MB_OK
            );
            return;
        }

        std::string dirPath = sharedMem->HCMDirPath;
       

        logging->initFileLogging(dirPath);
        logging->SetFileLoggingLevel(plog::verbose);
        PLOG_INFO << "HCMInternal initializing. DirPath: " << dirPath;
        unhandled = std::make_shared<UnhandledExceptionHandler>(dirPath); PLOGV << "unhandled init";
       //curl_global_init(CURL_GLOBAL_DEFAULT);
        try
        {
            // some very important services
            ModuleCache::initialize(); PLOGV << "moduleCache init"; // static singleton still.. blah
            auto mhm = std::make_unique<ModuleHookManager>(); PLOGV << "mhm init"; // is a static singleton still.. blah 
            auto ver = std::make_shared<GetMCCVersion>(); PLOGV << "ver init";// gets the version of MCC that we're currently injected into
            auto ptr = std::make_shared<PointerManager>(ver, dirPath); PLOGV << "ptr init"; // retrieves data from github page for MultilevelPointers and other game-version-specific data

            // setup some optional services mainly related to controls, eg freeing the cursor, pausing the game, etc
            auto control = std::make_shared<ControlServiceContainer>(ptr);


            // set up rendering
            auto d3d = std::make_shared<D3D11Hook>(); PLOGV << "d3d init"; // hooks d3d11 Present and ResizeBuffers
            auto imm = std::make_shared<ImGuiManager>(d3d, d3d->presentHookEvent); PLOGV << "imm init"; // sets up imgui context and fires off imgui render events

            auto mes = std::make_shared<MessagesGUI>(ImVec2{ 20, 20 }, imm->ForegroundRenderEvent); PLOGV << "mes init";// renders temporary messages to the screen
            auto exp = std::make_shared<RuntimeExceptionHandler>(mes); PLOGV << "exp init";// tells user if a cheat hook throws a runtime exception
            auto settings = std::make_shared<SettingsStateAndEvents>(std::make_shared<SettingsSerialiser>(dirPath, exp, mes)); PLOGV << "settings init";
            auto hke = std::make_shared<HotkeyEventsLambdas>(settings); // binds toggle hotkey events to lambdas of toggling settings etc
            auto mccStateHook = std::make_shared<MCCStateHook>(ptr, exp); PLOGV << "mccStateHook init";// fires event when game or level changes.
            auto guifail = std::make_shared<GUIServiceInfo>(mes); PLOGV << "guifail init"; // stores info about gui elements that failed to construct. starts empty, filled up later
            auto modal = std::make_shared<ModalDialogRenderer>(imm->ForegroundRenderEvent, control, settings->showGUIFailures, guifail); PLOGV << "modal init"; // renders modal dialogs that can be called from optionalCheats

            // hotkeys
            auto hkd = std::make_shared<HotkeyDefinitions>(settings); PLOGV << "hkd init";
            auto hkm = std::make_shared<HotkeyManager>(imm->ForegroundRenderEvent, hkd, mes, dirPath); PLOGV << "hkm init"; // hotkey manager doesn't render but wants to poll inputs every frame to know if keys pressed etc
            
            auto hkrimpl = std::make_unique<HotkeyRendererImpl>(imm->ForegroundRenderEvent, mes, hkm, hkd);
            auto hkr = std::make_shared<HotkeyRenderer>(std::move(hkrimpl)); PLOGV << "hkr init"; // render hotkeys and rebinding
            



            // set up optional cheats and optional gui elements
            auto guireq = std::make_shared<GUIRequiredServices>(); PLOGV << "guireq init"; // defines the gui elements we want to build and which optional cheats they will require
            auto cheatfail = std::make_shared<OptionalCheatInfo>(); PLOGV << "cheatfail init"; // stores info about failed optionalCheat construction (starts empty, obviously)
            auto optionalCheats = std::make_shared<OptionalCheatManager>(guireq, cheatfail, settings, ptr, ver, mccStateHook, sharedMem, mes, exp, dirPath, modal, control); PLOGV << "optionalCheats init"; // constructs and stores required optional cheats. Needs a lot of dependencies, cheats will only keep what they need.

            auto guistore = std::make_shared<GUIElementStore>(); PLOGV << "guistore init"; // collection starts empty, populated later by GUIElementConstructor
            auto GUICon = std::make_shared<GUIElementConstructor>(guireq, cheatfail, guistore, guifail, settings, ver->getMCCProcessType()); PLOGV << "GUIMan init"; // constructs gui elements, pushing them into guistore
            //guifail->printAllFailures();
            // set up main gui
            auto HCMGUI = std::make_shared<HCMInternalGUI>(mccStateHook, guistore, hkr, imm->MidgroundRenderEvent, mccStateHook->getMCCStateChangedEvent(), control, settings); PLOGV << "HCMGUI init";// main gui. Mostly just a canvas for rendering a collection of IGUIElements that will get constructed a bit below.
            mes->setAnchorPoint(HCMGUI);

            auto hb = std::make_shared<HeartbeatTimer>(sharedMem, settings); PLOGV << "hb init";

            d3d->beginHook();

            PLOG_INFO << "All services succesfully initialized! Entering main loop";
            Sleep(100);

            mes->addMessage("HCM successfully initialised!");

            std::thread modalFailureWindowThread;
            if (!guifail->getFailureMessagesMap().empty())
            {
                PLOG_DEBUG << "creating showFailedOptionalCheatServices modal dialog ";
                modalFailureWindowThread = std::thread{ ([modal = modal]() { Sleep(500); modal->showFailedOptionalCheatServices(); }) };
                modalFailureWindowThread.detach();
            }


            // We live in this loop 99% of the time
            while (!GlobalKill::isKillSet()) {
                Sleep(10);
            }
            PLOG_INFO << "HCMInternal services are about to fall out of scope";

            if (modalFailureWindowThread.joinable())
                modalFailureWindowThread.join();

        }
        catch (HCMInitException& ex) // mandatory services that fail to init will be caught here
        {

            std::ostringstream oss;
            oss << "\n\nHCMInternal failed initializing: " << ex.what() << std::endl
                << "Please send Burnt the log file located at: " << std::endl << logging->GetLogFileDestination();
            PLOG_FATAL << oss.str();

            int msgboxID = MessageBoxA(
                NULL,
                oss.str().c_str(),
                "Halo Checkpoint Manager error",
                MB_OK
            );

            GlobalKill::killMe();
        }
       // curl_global_cleanup(); PLOG_INFO << "Curl cleaned up";
        // Auto managed resources have fallen out of scope
        PLOG_INFO << "HCMInternal services successfully shut down";


#ifdef HCM_DEBUG
        PLOG_DEBUG << "Closing console";
        logging->closeConsole();
#endif 

	}


};