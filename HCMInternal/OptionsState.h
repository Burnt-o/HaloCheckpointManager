#pragma once
#include "Option.h"

typedef eventpp::CallbackList<void()> ActionEvent;

// really ought to turn this into a static class instead of namespace.. rn the shared_ptrs might not be cleared
namespace OptionsState
{


	inline std::shared_ptr<ActionEvent> forceCheckpointEvent = std::make_shared<eventpp::CallbackList<void()>>();
	inline std::shared_ptr<ActionEvent> forceRevertEvent = std::make_shared<eventpp::CallbackList<void()>>();
	inline std::shared_ptr<ActionEvent> forceDoubleRevertEvent = std::make_shared<eventpp::CallbackList<void()>>();
	inline std::shared_ptr<ActionEvent> forceCoreSaveEvent = std::make_shared<eventpp::CallbackList<void()>>();
	inline std::shared_ptr<ActionEvent> forceCoreLoadEvent = std::make_shared<eventpp::CallbackList<void()>>();

	inline std::shared_ptr<ActionEvent> injectCheckpointEvent = std::make_shared<eventpp::CallbackList<void()>>();
	inline std::shared_ptr<ActionEvent> dumpCheckpointEvent = std::make_shared<eventpp::CallbackList<void()>>();

	inline std::shared_ptr<ActionEvent> injectCoreEvent = std::make_shared<eventpp::CallbackList<void()>>();
	inline std::shared_ptr<ActionEvent> dumpCoreEvent = std::make_shared<eventpp::CallbackList<void()>>();



	inline std::vector<std::shared_ptr<SerialisableOption>> allSerialisableOptions{};
};

