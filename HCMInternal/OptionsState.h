#pragma once
#include "Option.h"



// really ought to turn this into a static class instead of namespace.. rn the shared_ptrs might not be cleared
namespace OptionsState
{


	inline std::shared_ptr<ActionEvent> forceCheckpointEvent = std::make_shared<ActionEvent>();
	inline std::shared_ptr<ActionEvent> forceRevertEvent = std::make_shared<ActionEvent>();
	inline std::shared_ptr<ActionEvent> forceDoubleRevertEvent = std::make_shared<ActionEvent>();
	inline std::shared_ptr<ActionEvent> forceCoreSaveEvent = std::make_shared<ActionEvent>();
	inline std::shared_ptr<ActionEvent> forceCoreLoadEvent = std::make_shared<ActionEvent>();

	inline std::shared_ptr<ActionEvent> injectCheckpointEvent = std::make_shared<eventpp::CallbackList<void()>>();
	inline std::shared_ptr<ActionEvent> dumpCheckpointEvent = std::make_shared<ActionEvent>();

	inline std::shared_ptr<ActionEvent> injectCoreEvent = std::make_shared<ActionEvent>();
	inline std::shared_ptr<ActionEvent> dumpCoreEvent = std::make_shared<ActionEvent>();

	inline std::shared_ptr<Option<bool>> speedhackToggle = std::make_shared<Option<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(speedhackToggle)
		);

	inline std::shared_ptr<Option<double>> speedhackSetting = std::make_shared<Option<double>>
		(
			10.f,
			[](float in) { return in > 0; }, // must be positive
			nameof(speedhackSetting)
		);

	inline std::shared_ptr<ActionEvent> speedhackHotkeyEvent = std::make_shared<ActionEvent>();

	//inline std::shared_ptr<ActionEvent> speedhackApply = std::make_shared<ActionEvent>();






	inline std::vector<std::shared_ptr<SerialisableOption>> allSerialisableOptions{};
};

