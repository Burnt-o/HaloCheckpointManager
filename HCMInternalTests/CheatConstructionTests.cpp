#pragma once
#include "pch.h"

#include "CheatConstructionTests.h"
//#include "map.h"
#include <boost\preprocessor.hpp>
#include <boost\preprocessor\control\if.hpp>
#include <boost\preprocessor\logical\or.hpp>
//#include "GuiElementEnum.h"
//#include "GUIRequiredServices.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace CheatConstructionTests
{
	static inline std::unique_ptr<CheatConstructionTestHarness> app;




	void testMCCStateHook(int mccVersion, MCCProcessType mccProcessType)
	{
		std::string dirPath = HCMINTERNAL_OUTPUT_PATH;
		dirPath += "\\";
		std::shared_ptr<MockGetMCCVersion> ver = std::make_shared<MockGetMCCVersion>(mccVersion, mccProcessType); PLOGV << "ver init";// gets the version of MCC that we're currently injected into
		std::shared_ptr<PointerManager> ptr = std::make_shared<PointerManager>(ver, dirPath); PLOGV << "ptr init"; // retrieves data from github page for MultilevelPointers and other game-version-specific data

		try
		{
			ptr->getData<std::shared_ptr<MultilevelPointer>>("gameEngineIndicator");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("levelIndicator");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("loadIndicator");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("menuIndicator");

			ptr->getData<std::shared_ptr<MultilevelPointer>>("gameLoadStartFunction");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("gameLoadEndFunction");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("gameQuitFunction");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("gameEngineChangedFunction");
		}
		catch (HCMInitException ex)
		{
			Assert::Fail(str_to_wstr(ex.what()).c_str());
		}
	}


#define MAKE_CHEATTEST_PAIR(r, ELEMENT, i, GAME) \
TEST_METHOD(BOOST_PP_CAT(GAME, ELEMENT))\
		{\
		Assert::IsTrue(app->guistore->mapOfSuccessfullyConstructedGUIElements.at(GameState::Value::GAME).contains(GUIElementEnum::ELEMENT), \
		str_to_wstr(app->guifail->getFailure(GUIElementEnum::ELEMENT, GameState::Value::GAME)).c_str());\
		}\



#define MAKE_CHEATTESTPAIR_SET(r, d, seq) BOOST_PP_SEQ_FOR_EACH_I(MAKE_CHEATTEST_PAIR, BOOST_PP_TUPLE_ELEM(0, seq), BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_ELEM(1, seq)))
#define TEST_ALL_CHEATPAIRS(seq) BOOST_PP_SEQ_FOR_EACH(MAKE_CHEATTESTPAIR_SET, _, seq)



#define TEST_MCC_VERSION_WINSTORE(MCC_VERSION) 	\
TEST_CLASS(MCCVerWinstore##MCC_VERSION)\
{ \
	TEST_CLASS_INITIALIZE(init##MCC_VERSION)\
	{\
	PLOG_DEBUG << "destroying old app";\
	app.reset();\
	PLOG_DEBUG << "creating new app";\
		app = std::make_unique<CheatConstructionTestHarness>(MCC_VERSION, MCCProcessType::WinStore);\
	}\
	TEST_ALL_CHEATPAIRS(ALLGUIELEMENTS_ANDSUPPORTEDGAMES)\
	TEST_METHOD(MCCStateHook##MCC_VERSION)\
	{\
		testMCCStateHook(MCC_VERSION, MCCProcessType::WinStore);\
	}\
}\


#define TEST_MCC_VERSION_STEAM(MCC_VERSION) 	\
TEST_CLASS(MCCVerSteam##MCC_VERSION)\
{ \
	TEST_CLASS_INITIALIZE(init##MCC_VERSION)\
	{\
	PLOG_DEBUG << "destroying old app";\
	app.reset();\
	PLOG_DEBUG << "creating new app";\
		app = std::make_unique<CheatConstructionTestHarness>(MCC_VERSION, MCCProcessType::Steam);\
	}\
	TEST_ALL_CHEATPAIRS(ALLGUIELEMENTS_ANDSUPPORTEDGAMES)\
	TEST_METHOD(MCCStateHook##MCC_VERSION)\
	{\
		testMCCStateHook(MCC_VERSION, MCCProcessType::Steam);\
	}\
}\





	// where the magic happens (hope you don't throw a compiler out of heap space warning)
	TEST_MCC_VERSION_STEAM(2448); // Season 7
	TEST_MCC_VERSION_STEAM(2645); // Season 8
	//TEST_MCC_VERSION_STEAM(3073); // I don't think anyone actually runs on 3073 so ceebs
	TEST_MCC_VERSION_STEAM(3272); // Current patch
	TEST_MCC_VERSION_WINSTORE(3272); // Current patch




}