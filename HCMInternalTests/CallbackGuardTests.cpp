#include "pch.h"
#include "ScopedCallback.h"

std::wstring str_to_wstr(const std::string str)
{
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wStr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wStr, wchars_num);
	return std::wstring(wStr);
}

// https://cplusplus.com/reference/condition_variable/condition_variable/

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HCMInternalTests
{
	TEST_CLASS(CallbackGuardTests)
	{
		TEST_METHOD_INITIALIZE(init)
		{
			TestLogging log;
		}





		struct EventFirer 
		{
#define useSeperateThread 0

			std::shared_ptr<ActionEvent> theEvent = std::make_shared<ActionEvent>();
			void fireTheEvent() 
			{ 
#if useSeperateThread == 1
				// fire on seperate thread
				PLOG_DEBUG << "firing event on seperate event";
				std::thread t([this]() {theEvent->operator()(); });
				t.detach();
				PLOG_DEBUG << "event has been fired on seperate thread";
#else
				theEvent->operator()();
#endif
			}
		};

		struct EventListener // contains the callback
		{
			std::unique_ptr<int> shouldBe420 = std::make_unique<int>(420); // some data member the callback needs to access
			int give420() { return *shouldBe420.get(); }
			ScopedCallback<ActionEvent> eventCallbackHandle;

			bool alive = true;


			std::mutex mtx{};


			EventListener(std::shared_ptr<ActionEvent> theEvent)
				: eventCallbackHandle(theEvent, [this]() { callbackMethod(); }) {}

			~EventListener() {
				std::scoped_lock<std::mutex> lck(mtx);
	

				PLOG_DEBUG << "~EventListener"; shouldBe420.reset(); alive
					= false;
			}




			void callbackMethod()
			{
				std::scoped_lock<std::mutex> lck(mtx);
	
				PLOG_DEBUG << "beginning callbackMethod";


				Sleep(10); // simulate expensive task


				PLOG_DEBUG << "testing data members";
				Assert::IsTrue(shouldBe420.operator bool(), L"shouldBe420 null");
				Assert::AreEqual(420, *shouldBe420.get(), L"shouldBe420 not equal to 420");
				Assert::IsNotNull(this, L"this is null");
				Assert::IsTrue(alive, L"alive is false");
				Assert::AreEqual(420, give420(), L"give420 did not return 420");

				PLOG_DEBUG << "all good";

			}
		};


		// don't call callback after containing class destroyed
		TEST_METHOD(CallbackAfterDestruction)
		{
			EventFirer eventFirer;

			std::shared_ptr<EventListener> eventListener = std::make_shared<EventListener>(eventFirer.theEvent);
			eventFirer.fireTheEvent();

			Sleep(20);
			PLOG_DEBUG << "killing eventListener";
			std::thread t([el = eventListener]() { el.~shared_ptr(); });
			t.detach();
			Sleep(10);
			PLOG_DEBUG << "firing event at dead listener";
			eventFirer.fireTheEvent();
			Sleep(30);



		}

		// don't destroy class while callback running
		TEST_METHOD(DestructionDuringCallback)
		{
			EventFirer eventFirer;
			std::shared_ptr<EventListener> eventListener = std::make_shared<EventListener>(eventFirer.theEvent);
			std::thread t([el = eventListener]() { PLOG_DEBUG << "killing listener"; el.~shared_ptr(); });
			t.detach();
			eventFirer.fireTheEvent();

			Sleep(30);

		
		}
	};

}
