#include "pch.h"
#include "CppUnitTest.h"
#include "ScopedCallback2.h"
#include "TestLogging.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HCMInternalTests
{
	TEST_CLASS(ScopedCallbackTests)
	{
		TEST_METHOD_INITIALIZE(init)
		{
			TestLogging log;
		}

	public:

		class HitCounter
		{
		private: 
			int hitCount = 0;
		public:
			void incrementHitCount() { hitCount++; };
			int getHitCount() { return hitCount; }
		};


		class EventFirer
		{

		public:
			void launchZeMissles() { mEvent->operator()(); }
			std::shared_ptr<ActionEvent> mEvent = std::make_shared<ActionEvent>();
		};


		class SomeDumbInterface
		{
		public:
			virtual std::string_view getName() = 0;
		};

		class Listener : public SomeDumbInterface
		{
			int shouldBe420;
			void ohGodTheHumanity() 
			{ 
				Assert::AreEqual(shouldBe420, 420);
				mHitCounter->incrementHitCount();
				PLOG_VERBOSE << "AHHHHH!"; 
			}
			ScopedCallback2<ActionEvent> mMissleCallback;
			std::shared_ptr< HitCounter> mHitCounter;
		public:
			Listener(std::shared_ptr<ActionEvent> missileEvent, std::shared_ptr<HitCounter> hitCounter, bool throws = false)
				:mHitCounter(hitCounter), mMissleCallback(missileEvent, [this]() { ohGodTheHumanity(); })
			{
				shouldBe420 = 420;
				//mMissleCallback = { missileEvent, [this]() { ohGodTheHumanity(); } };
				if (throws) throw std::exception("throwing");
			}
			~Listener() { PLOG_VERBOSE << "rip me"; }

			virtual std::string_view getName() override { return "Eeeee"; };
		};



		TEST_METHOD(EventsFireCorrectly)
		{
			auto hitCounter = std::make_shared<HitCounter>();
			EventFirer eventFirer;
			Assert::IsTrue(eventFirer.mEvent->empty());
			Listener listener(eventFirer.mEvent, hitCounter);
			Assert::IsFalse(eventFirer.mEvent->empty());
			eventFirer.launchZeMissles(); // triple barrage for maximum damage
			eventFirer.launchZeMissles();
			eventFirer.launchZeMissles();
			Assert::AreEqual(3, hitCounter->getHitCount());
		}

		
		TEST_METHOD(ScopedSelfDestructs)
		{
			PLOG_DEBUG << "hi";
			EventFirer eventFirer;
			auto hitCounter = std::make_shared<HitCounter>();
			// should be no listeners right now
			Assert::IsTrue(eventFirer.mEvent->empty());

			// bad listener self destructs
			try
			{
				PLOG_DEBUG << "Making bad listener";
				Listener badlistener(eventFirer.mEvent, hitCounter, true);
			}
			catch(std::exception ex)
			{
			}
			PLOG_DEBUG << "bad listener aborted";
			Assert::IsTrue(eventFirer.mEvent->empty());

			{ // listener scope
				PLOG_DEBUG << "making good listener";
				Listener listener(eventFirer.mEvent, hitCounter);
				//Assert::IsFalse(eventFirer.mEvent->empty());
				eventFirer.launchZeMissles(); // triple barrage for maximum damage
				eventFirer.launchZeMissles();
				eventFirer.launchZeMissles();
				
				Assert::AreEqual(3, hitCounter->getHitCount());
				PLOG_DEBUG << "good listener about to fall out of scope";
			}
			PLOG_DEBUG << "good listener dead";
			// listener is dead now. fire some more missles anyway just to be sure
			eventFirer.launchZeMissles();
			eventFirer.launchZeMissles();
			Assert::AreEqual(3, hitCounter->getHitCount()); // last two hits were not good since already dead
		}
	};
}
