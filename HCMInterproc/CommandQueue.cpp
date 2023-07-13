#include "pch.h"
#include "CommandQueue.h"

HCMExternalCommand currentCommand;

void SetCommand(HCMExternalCommand command)
{
	currentCommand = command;
}

HCMExternalCommand GetCommand()
{
	HCMExternalCommand copy = currentCommand;
	currentCommand = HCMExternalCommand::None;
	return copy;
}