#pragma once
#include "IGetMCCVersion.h"
class MockGetMCCVersion : public IGetMCCVersion {

	VersionInfo mockVersionInfo;
	std::string mockVersionInfoString;
	MCCProcessType mockProcessType;

	public:
		MockGetMCCVersion() = delete;
		MockGetMCCVersion(int mccVersion, MCCProcessType mccProcessType)
			: mockProcessType(mccProcessType)
		{
			PLOG_DEBUG << "MockGetMCCVersion con";
			mockVersionInfo = { 1, (ULONG)mccVersion, 0, 0 };

			std::stringstream ss;
			ss << mockVersionInfo;
			mockVersionInfoString = ss.str();
		}

		~MockGetMCCVersion() { PLOG_DEBUG << "~MockGetMCCVersion"; }

		virtual VersionInfo getMCCVersion() override { return mockVersionInfo; }
		virtual std::string_view getMCCVersionAsString() override { return mockVersionInfoString;  }
		virtual MCCProcessType getMCCProcessType() override { return mockProcessType; }

};