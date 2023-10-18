#pragma once
#include "Setting.h"
#include "ISettingsSerialiser.h"
class SettingsStateAndEvents
{
private:
	std::shared_ptr<ISettingsSerialiser> mSerialiser;

public:
	SettingsStateAndEvents(std::shared_ptr<ISettingsSerialiser> serialiser)
		: mSerialiser(serialiser)
	{ 
		mSerialiser->deserialise(allSerialisableOptions); 
	
	}
	~SettingsStateAndEvents() {
		PLOG_DEBUG << "~SettingsStateAndEvents()";
		mSerialiser->serialise(allSerialisableOptions); 
	};

	//	hotkeys
	std::shared_ptr<ActionEvent> toggleGUIHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> togglePauseHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> speedhackHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> invulnerabilityHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> aiFreezeHotkeyEvent = std::make_shared<ActionEvent>();


	// events
	std::shared_ptr<ActionEvent> forceCheckpointEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceRevertEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceDoubleRevertEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceCoreSaveEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceCoreLoadEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> injectCheckpointEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> dumpCheckpointEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> injectCoreEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> dumpCoreEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> consoleCommandEvent = std::make_shared<ActionEvent>();


	// settings
	std::shared_ptr<Setting<bool>> GUIWindowOpen = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIWindowOpen)
		);

	std::shared_ptr<Setting<bool>> GUIShowingFreesCursor = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIShowingFreesCursor)
		);


	std::shared_ptr<Setting<bool>> GUIShowingBlocksInput = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIShowingBlocksInput)
		);

	std::shared_ptr<Setting<bool>> GUIShowingPausesGame = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIShowingPausesGame)
		);

	std::shared_ptr<Setting<bool>> togglePause = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(togglePause)
		);

	std::shared_ptr<Setting<bool>> pauseAlsoBlocksInput = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(pauseAlsoBlocksInput)
		);

	std::shared_ptr<Setting<bool>> pauseAlsoFreesCursor = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(pauseAlsoFreesCursor)
		);

	std::shared_ptr<Setting<bool>> speedhackToggle = std::make_shared<Setting<bool>>
	(
		false,
		[](bool in) { return true; },
		nameof(speedhackToggle)
	);

	std::shared_ptr<Setting<double>> speedhackSetting = std::make_shared<Setting<double>>
	(
		10.f,
		[](float in) { return in > 0; }, // must be positive
		nameof(speedhackSetting)
	);


	std::shared_ptr<Setting<bool>> invulnerabilityToggle = std::make_shared<Setting<bool>>
	(
		false,
		[](bool in) { return true; },
		nameof(invulnerabilityToggle)
	);

	std::shared_ptr<Setting<bool>> invulnerabilityNPCToggle = std::make_shared<Setting<bool>>
	(
		false,
		[](bool in) { return true; },
		nameof(invulnerabilityNPCToggle)
	);

	std::shared_ptr<Setting<bool>> aiFreezeToggle = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(aiFreezeToggle)
		);

	std::shared_ptr<Setting<std::string>> consoleCommandString = std::make_shared<Setting<std::string>>
		(
			"game_revert",
			[](std::string in) { return true; },
			nameof(consoleCommandString)
		);

	// settings that ought to be serialised/deserialised between HCM runs
	std::vector<std::shared_ptr<SerialisableSetting>> allSerialisableOptions
	{
		invulnerabilityNPCToggle, 
		speedhackSetting, 
		GUIShowingFreesCursor, 
		GUIShowingBlocksInput, 
		GUIShowingPausesGame, 
		pauseAlsoBlocksInput,
		pauseAlsoFreesCursor,
		consoleCommandString
	};
};

