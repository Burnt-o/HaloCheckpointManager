#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ObservedEvent.h"


enum class HSOutputType
{
	Normal,
	Error
};

using HSOutputEvent = eventpp::CallbackList<void(const std::string&, const HSOutputType&)>;

class HaloScriptOutputHookEventImpl {
public:
	virtual ~HaloScriptOutputHookEventImpl() = default;
	virtual std::shared_ptr<ObservedEvent<HSOutputEvent>> getHaloScriptOutputEvent() = 0;
};
class HaloScriptOutputHookEvent : public IOptionalCheat
{
private:

	std::unique_ptr<HaloScriptOutputHookEventImpl> pimpl;
public:
	HaloScriptOutputHookEvent(GameState game, IDIContainer& dicon);
	~HaloScriptOutputHookEvent();

	std::shared_ptr<ObservedEvent<HSOutputEvent>> getHaloScriptOutputEvent() { return pimpl->getHaloScriptOutputEvent(); }

	virtual std::string_view getName() override { return nameof(HaloScriptOutputHookEvent); }
};

