#include "pch.h"
#include "ModalDialogFactory.h"
#include "FailedOptionalCheatServicesDialog.h"
#include "InjectionWarningDialog.h"
#include "CheckpointDumpNameDialog.h"
#include "EditOrAddWaypointDialog.h"
#include "TriggerFilterStringDialog.h"
#include "EditOrAddViewAngleLineDialog.h"



	std::shared_ptr<IModalDialogVoid> ModalDialogFactory::makeFailedOptionalCheatServicesDialog
	(std::shared_ptr<GUIServiceInfo> guiFailures)
	{
		PLOG_VERBOSE << "factory creating FailedOptionalCheatServicesDialog";
		return std::make_unique<FailedOptionalCheatServicesDialog>(guiFailures);
	}

	std::shared_ptr < IModalDialogReturner<bool>> ModalDialogFactory::makeInjectionWarningDialog
	(std::string dialogTitle, std::string warningText)
	{
		PLOG_VERBOSE << "factory creating InjectionWarningDialog";
		return std::make_unique<InjectionWarningDialog>(dialogTitle, warningText);
	}

	std::shared_ptr<IModalDialogReturner<std::tuple<bool, std::string>>> ModalDialogFactory::makeCheckpointDumpNameDialog
	(std::string dialogTitle, std::string defaultValue)
	{
		PLOG_VERBOSE << "factory creating CheckpointDumpNameDialog";
		return std::make_unique<CheckpointDumpNameDialog>(dialogTitle, defaultValue);
	}

	std::shared_ptr<IModalDialogReturner<std::optional<Waypoint>>> ModalDialogFactory::makeEditOrAddWaypointDialog
	(std::shared_ptr<RuntimeExceptionHandler> runtimeExceptionHandler, std::string dialogTitle, Waypoint defaultWaypoint, std::optional<SimpleMath::Vector3> playerPosition, bool canMeasureDistance)
	{
		PLOG_VERBOSE << "factory creating EditOrAddWaypointDialog";
		return std::make_unique<EditOrAddWaypointDialog>(runtimeExceptionHandler, dialogTitle, defaultWaypoint, playerPosition, canMeasureDistance);
	}

	std::shared_ptr<IModalDialogReturner<std::optional<ViewAngleLine>>> ModalDialogFactory::makeEditOrAddViewAngleLineDialog
	(std::shared_ptr<RuntimeExceptionHandler> runtimeExceptionHandler, std::string dialogTitle, ViewAngleLine defaultViewAngleLine, std::optional<SubpixelID> playerAngle)
	{
		PLOG_VERBOSE << "factory creating EditOrAddViewAngleLineDialog";
		return std::make_unique<EditOrAddViewAngleLineDialog>(runtimeExceptionHandler, dialogTitle, defaultViewAngleLine, playerAngle);
	}


	std::shared_ptr<IModalDialogReturner<std::string>> ModalDialogFactory::makeTriggerFilterStringDialog
	(std::string dialogTitle, std::string defaultValue, std::string allTriggers, LevelID currentLevel, std::optional<std::shared_ptr<std::map<LevelID, std::vector<std::pair<std::string, std::string>>>>> levelMapStringVector)
	{
		PLOG_VERBOSE << "factory creating TriggerFilterStringDialog";
		return std::make_unique<TriggerFilterStringDialog>(dialogTitle, defaultValue, allTriggers, currentLevel, levelMapStringVector);
	}