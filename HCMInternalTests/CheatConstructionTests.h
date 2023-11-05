#pragma once
#include "CppUnitTest.h"
#include "TestLogging.h"
#include "curl\curl.h"
#include "GlobalKill.h"
#include "ModuleCache.h"
#include "ModuleHookManager.h"

#include "PointerManager.h"
#include "MCCStateHook.h"
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
#include "DynamicStructFactory.h"
#include "RuntimeExceptionHandler.h"
#include "MockMessagesGUI.h"
#include "MockSharedMemory.h"
#include "MockGetMCCVersion.h"
#include "MockSettingsSerialiser.h"
#include "MockMCCStateHook.h"
#include "MockModalDialogRenderer.h"
#include "ControlServiceContainer.h"
	class CheatConstructionTestHarness
	{
	public:
		static inline bool ready = false;

		std::shared_ptr<GUIElementStore> guistore;
		std::shared_ptr<GUIRequiredServices> guireq;
		std::shared_ptr<GUIServiceInfo> guifail;

		~CheatConstructionTestHarness()
		{
			curl_global_cleanup();
		}
		CheatConstructionTestHarness(int mccVersion, MCCProcessType mccProcessType)
		{
			ready = false;
			TestLogging log;
			PLOG_DEBUG << "starting";
			curl_global_init(CURL_GLOBAL_DEFAULT);
			std::string dirPath = HCMINTERNAL_OUTPUT_PATH;
			dirPath += "\\";
			auto unhandled = std::make_shared<UnhandledExceptionHandler>(dirPath); PLOGV << "unhandled init";
			auto sharedMem = std::make_shared< MockSharedMemory>(dirPath);

			//ModuleCache::initialize(); PLOGV << "moduleCache init"; // static singleton still.. blah
			//auto mhm = std::make_unique<ModuleHookManager>(); PLOGV << "mhm init"; // is a static singleton still.. blah 

			auto ver = std::make_shared<MockGetMCCVersion>(mccVersion, mccProcessType); PLOGV << "ver init";// gets the version of MCC that we're currently injected into
			auto ptr = std::make_shared<PointerManager>(ver, dirPath); PLOGV << "ptr init"; // retrieves data from github page for MultilevelPointers and other game-version-specific data

			auto mes = std::make_shared<MockMessagesGUI>();
			auto exp = std::make_shared<RuntimeExceptionHandler>(mes); PLOGV << "exp init";// tells user if a cheat hook throws a runtime exception

			auto settings = std::make_shared<SettingsStateAndEvents>(std::make_shared<MockSettingsSerialiser>()); PLOGV << "settings init";

			auto mccStateHook = std::make_shared<MockMCCStateHook>(); PLOGV << "mccStateHook init";// fires event when game or level changes.

			auto modal = std::make_shared<MockModalDialogRenderer>(); PLOGV << "modal init";
			auto control = std::make_shared<ControlServiceContainer>(ptr);

			// set up optional cheats and optional gui elements
			guireq = std::make_shared<GUIRequiredServices>(); PLOGV << "guireq init"; // defines the gui elements we want to build and which optional cheats they will require
			auto cheatfail = std::make_shared<OptionalCheatInfo>(); PLOGV << "cheatfail init"; // stores info about failed optionalCheat construction (starts empty, obviously)
			auto optionalCheats = std::make_shared<OptionalCheatManager>(guireq, cheatfail, settings, ptr, ver, mccStateHook, sharedMem, mes, exp, dirPath, modal, control); PLOGV << "optionalCheats init"; // constructs and stores required optional cheats. Needs a lot of dependencies, cheats will only keep what they need.
			guifail = std::make_shared<GUIServiceInfo>(mes); PLOGV << "guifail init"; // stores info about gui elements that failed to construct. starts empty
			guistore = std::make_shared<GUIElementStore>(); PLOGV << "guistore init"; // collection starts empty, populated later by GUIElementConstructor
			auto GUICon = std::make_shared<GUIElementConstructor>(guireq, cheatfail, guistore, guifail, settings); PLOGV << "GUIMan init"; // constructs gui elements, pushing them into guistore
			//guifail->printAllFailures();

			ready = true;

		}

	};




