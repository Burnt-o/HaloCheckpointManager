#pragma once
#include "pch.h"

#include "CheatConstructionTests.h"
//#include "map.h"
#include <boost\preprocessor.hpp>
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
			ptr->getData<std::shared_ptr<MultilevelPointer>>("gameIndicator");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("levelIndicator");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("loadIndicator");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("menuIndicator");

			ptr->getData<std::shared_ptr<MultilevelPointer>>("gameLoadStartFunction");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("gameLoadEndFunction");
			ptr->getData<std::shared_ptr<MultilevelPointer>>("gameQuitFunction");
		}
		catch (HCMInitException ex)
		{
			Assert::Fail(str_to_wstr(ex.what()).c_str());
		}
	}


	void testFreeCursorService(int mccVersion, MCCProcessType mccProcessType)
	{
		std::string dirPath = HCMINTERNAL_OUTPUT_PATH;
		dirPath += "\\";
		std::shared_ptr<MockGetMCCVersion> ver = std::make_shared<MockGetMCCVersion>(mccVersion, mccProcessType); PLOGV << "ver init";// gets the version of MCC that we're currently injected into
		std::shared_ptr<PointerManager> ptr = std::make_shared<PointerManager>(ver, dirPath); PLOGV << "ptr init"; // retrieves data from github page for MultilevelPointers and other game-version-specific data

		try
		{
			auto freeCursorService = std::make_optional< std::shared_ptr<FreeMCCCursor>>(std::make_shared<FreeMCCCursor>
				(ptr->getData<std::shared_ptr<MultilevelPointer>>("shouldCursorBeFreeFunction"),
					ptr->getData <std::shared_ptr<MidhookFlagInterpreter>>("shouldCursorBeFreeFunctionFlagSetter")));
		}
		catch (HCMInitException ex)
		{
			Assert::Fail(str_to_wstr(ex.what()).c_str());
		}
	}


#define MAKE_CHEATTEST_PAIR(r, ELEMENT, i, GAME) TEST_METHOD(BOOST_PP_CAT(GAME, ELEMENT))\
		{\
		Assert::IsTrue(app->guistore->mapOfSuccessfullyConstructedGUIElements.at(GameState::Value::GAME).contains(GUIElementEnum::ELEMENT), \
		str_to_wstr(app->guifail->getFailure(GUIElementEnum::ELEMENT, GameState::Value::GAME)).c_str());\
		}\


#define MAKE_CHEATTESTPAIR_SET(r, d, seq) BOOST_PP_SEQ_FOR_EACH_I(MAKE_CHEATTEST_PAIR, BOOST_PP_TUPLE_ELEM(0, seq), BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_ELEM(1, seq)))
#define TEST_ALL_CHEATPAIRS(seq) BOOST_PP_SEQ_FOR_EACH(MAKE_CHEATTESTPAIR_SET, _, seq)



#define TEST_MCC_VERSION_STEAMANDWINSTORE(MCC_VERSION) 	\
TEST_CLASS(MCCVer##MCC_VERSION)\
{ \
	TEST_CLASS_INITIALIZE(init##MCC_VERSION)\
	{\
	PLOG_DEBUG << "destroying old app";\
	app.reset();\
	PLOG_DEBUG << "creating new app";\
		app = std::make_unique<CheatConstructionTestHarness>(MCC_VERSION, MCCProcessType::Steam);\
	}\
	TEST_ALL_CHEATPAIRS(ALLGUIELEMENTS_ANDSUPPORTEDGAMES)\
	TEST_METHOD(steamMCCStateHook##MCC_VERSION)\
	{\
		testMCCStateHook(MCC_VERSION, MCCProcessType::Steam);\
	}\
	TEST_METHOD(winstoreMCCStateHook##MCC_VERSION)\
	{\
		testMCCStateHook(MCC_VERSION, MCCProcessType::WinStore);\
	}\
	TEST_METHOD(steamFreeMCCCursor##MCC_VERSION)\
	{\
		testFreeCursorService(MCC_VERSION, MCCProcessType::Steam);\
	}\
	TEST_METHOD(winstoreFreeMCCCursor##MCC_VERSION)\
	{\
		testFreeCursorService(MCC_VERSION, MCCProcessType::WinStore);\
	}\
}\


#define TEST_MCC_VERSION_STEAMONLY(MCC_VERSION) 	\
TEST_CLASS(MCCVer##MCC_VERSION)\
{ \
	TEST_CLASS_INITIALIZE(init##MCC_VERSION)\
	{\
	PLOG_DEBUG << "destroying old app";\
	app.reset();\
	PLOG_DEBUG << "creating new app";\
		app = std::make_unique<CheatConstructionTestHarness>(MCC_VERSION, MCCProcessType::Steam);\
	}\
	TEST_ALL_CHEATPAIRS(ALLGUIELEMENTS_ANDSUPPORTEDGAMES)\
	TEST_METHOD(steamMCCStateHook##MCC_VERSION)\
	{\
		testMCCStateHook(MCC_VERSION, MCCProcessType::Steam);\
	}\
	TEST_METHOD(steamFreeMCCCursor##MCC_VERSION)\
	{\
		testFreeCursorService(MCC_VERSION, MCCProcessType::Steam);\
	}\
}\





	// where the magic happens (hope you don't throw a compiler out of heap space warning)
	TEST_MCC_VERSION_STEAMONLY(2448); // Season 7
	TEST_MCC_VERSION_STEAMONLY(2645); // Season 8
	//TEST_MCC_VERSION(3073); // I don't think anyone actually runs on 3073 so ceebs
	TEST_MCC_VERSION_STEAMANDWINSTORE(3272); // Current patch




}