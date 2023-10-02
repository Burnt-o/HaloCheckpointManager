#pragma once
#include "Setting.h"
#include "SettingsSerialiser.h"
class SettingsStateAndEvents
{
private:
	std::shared_ptr<SettingsSerialiser> mSerialiser;
public:
	SettingsStateAndEvents(std::shared_ptr<SettingsSerialiser> serialiser) 
		: mSerialiser(serialiser) 
	{ 
		mSerialiser->deserialise(allSerialisableOptions); 
	
	}
	~SettingsStateAndEvents() {
		PLOG_DEBUG << "~SettingsStateAndEvents()";
		mSerialiser->serialise(allSerialisableOptions); 
	};

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
	std::shared_ptr<ActionEvent> speedhackHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> invulnerabilityHotkeyEvent = std::make_shared<ActionEvent>();

	// settings
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


	// settings that ought to be serialised/deserialised between HCM runs
	std::vector<std::shared_ptr<SerialisableSetting>> allSerialisableOptions{invulnerabilityNPCToggle, speedhackSetting};
};

