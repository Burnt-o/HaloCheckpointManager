#pragma once
#include "IModalDialog.h"
#include "WaypointList.h"
#include "GUIServiceInfo.h"
#include "GetPlayerDatum.h"
#include "GetObjectPhysics.h"
#include "RuntimeExceptionHandler.h"
#include "MCCState.h"

// hides IModalDialog concrete classes. Just passes onto concrete constructor and returns as interface
namespace ModalDialogFactory 
{
	std::shared_ptr<IModalDialogVoid> makeFailedOptionalCheatServicesDialog(std::shared_ptr<GUIServiceInfo> guiFailures);
	std::shared_ptr < IModalDialogReturner<bool>> makeInjectionWarningDialog(std::string dialogTitle, std::string warningText);
	std::shared_ptr<IModalDialogReturner<std::tuple<bool, std::string>>> makeCheckpointDumpNameDialog(std::string dialogTitle, std::string defaultValue);
	std::shared_ptr<IModalDialogReturner<std::optional<Waypoint>>> makeEditOrAddWaypointDialog(std::shared_ptr<RuntimeExceptionHandler> runtimeExceptionHandler, std::string dialogTitle, Waypoint defaultWaypoint, std::optional<SimpleMath::Vector3> playerPosition, bool canMeasureDistance);
	std::shared_ptr<IModalDialogReturner<std::string>> makeTriggerFilterStringDialog(std::string dialogTitle, std::string defaultValue, MCCState mccState, std::string allTriggers);
}
