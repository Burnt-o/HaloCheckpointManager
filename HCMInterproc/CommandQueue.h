#pragma once

enum HCMExternalCommand {
	None = 0,
	Inject = 1,
	Dump = 2
};

extern "C" _declspec(dllexport) void SetCommand(HCMExternalCommand command);
extern HCMExternalCommand GetCommand();