#include "pch.h"
#include "DIContainer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HCMInternalTests
{
	TEST_CLASS(DIContainerTests)
	{
		TEST_METHOD_INITIALIZE(init)
		{
			TestLogging log;
		}
	public:

		// insert a service into a dicontainer and resolve it
		TEST_METHOD(ImplInjectionTest)
		{
			struct ServiceMock
			{
				int return69() { return 69; } // nice
			};

			struct NeedyMock
			{
				NeedyMock(IDIContainer& dicon)
				{
					auto serv = dicon.Resolve<ServiceMock>();
					Assert::IsTrue(serv.operator bool());

					auto my69 = serv->return69();
					Assert::AreEqual(my69, 69);
				}
			};


			// construct serviceMock, construct dicontainer, register ServiceMock, pass it to NeedyMock
			auto serviceMock = std::make_shared<ServiceMock>();
			DIContainer<ServiceMock> dicon = { serviceMock };
			auto needyMock = std::make_shared< NeedyMock>(dicon);
		}

		// insert a derived service into a dicontainer and resolve it as a base interface
		TEST_METHOD(InterfaceInjectionTest)
		{
			struct ServiceMockInterface
			{
				virtual int return69() = 0;
			};

			struct ServiceMockImpl : public ServiceMockInterface
			{
				virtual int return69() override { return 69; } // nice
			};

			struct NeedyMock
			{
				NeedyMock(IDIContainer& dicon)
				{
					auto serv = dicon.Resolve<ServiceMockInterface>();
					Assert::IsTrue(serv.operator bool());

					auto my69 = serv->return69();
					Assert::AreEqual(my69, 69);
				}
			};


			// construct ServiceMockImpl, construct dicontainer, register it as interface, pass it to NeedyMock which resolves it as ServiceMockInterface
			auto serviceMock = std::make_shared<ServiceMockImpl>();
			DIContainer<ServiceMockInterface> dicon = { serviceMock };
			auto needyMock = std::make_shared< NeedyMock>(dicon);
		}
	};
}
