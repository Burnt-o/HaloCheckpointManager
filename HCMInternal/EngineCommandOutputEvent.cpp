#include "pch.h"
#include "EngineCommandOutputEvent.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"
#include "MidhookContextInterpreter.h"
#include "RuntimeExceptionHandler.h"
#include "ObservedEventFactory.h"

template <GameState::Value gameT>
class EngineCommandOutputEventImplT : public EngineCommandOutputEventImpl
{
private:
	// static stuff for hook
	static inline std::mutex mDestructionGuard{};
	static inline EngineCommandOutputEventImplT<gameT>* instance = nullptr;

	// main event we provide
	std::shared_ptr<ObservedEvent<EngineOutputEvent>> mEngineOutputEvent;
	std::unique_ptr<ScopedCallback<ActionEvent>> mEngineOutputEventSubscribersChangedCallback;


	// data
	std::shared_ptr<ModuleMidHook> commandOutputStringHook;
	std::shared_ptr<MidhookContextInterpreter> commandOutputStringFunctionContext;

	// injected services
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;


	void onEngineOutputEventSubscribersChanged()
	{
		// disable hook if callbacklist is empty (no subscribers), enable otherwise
		commandOutputStringHook->setWantsToBeAttached(mEngineOutputEvent->isEventSubscribed());
	}



	// for valid output
	static void commandOutputStringHookFunction(SafetyHookContext& ctx)
	{
		if (!instance)
			return;

		std::unique_lock<std::mutex> lock(mDestructionGuard);
		LOG_ONCE(PLOG_DEBUG << "commandOutputStringHookFunction running");

		try
		{
			enum class param
			{
				pOutputString
			};
			auto* ctxInterpreter = instance->commandOutputStringFunctionContext.get();


			try
			{
				// will be nullptr if there was no error. In which case return early.
				uintptr_t outputCharsPointer = (uintptr_t)ctxInterpreter->getParameterRef(ctx, (int)param::pOutputString);
				if (outputCharsPointer == 0)
				{
					PLOG_DEBUG << "no output";
					return;
				}

				PLOG_DEBUG << "outputCharsPointer: " << std::hex << outputCharsPointer;
			}
			catch (HCMRuntimeException ex)
			{
				PLOG_DEBUG << "error parsing outputCharsPointer: " << ex.what();
				return;
			}

			const char* outputChars = (const char*)ctxInterpreter->getParameterRef(ctx, (int)param::pOutputString);
			if (IsBadReadPtr(outputChars, 4))
				throw HCMRuntimeException(std::format("Bad read of command string output characters! at {:X}", (uintptr_t)outputChars));

			auto out = std::string(outputChars);
			PLOG_DEBUG << "Firing engine output event with string: " << out;
			instance->mEngineOutputEvent->fireEvent(out);
		}
		catch (HCMRuntimeException ex)
		{
			instance->runtimeExceptions->handleMessage(ex);
		}
	}



public:
	EngineCommandOutputEventImplT(GameState game, IDIContainer& dicon)
		: runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{

		mEngineOutputEvent = ObservedEventFactory::makeObservedEvent<EngineOutputEvent>();
		mEngineOutputEventSubscribersChangedCallback = ObservedEventFactory::getCallbackListChangedCallback(mEngineOutputEvent, [this]() {onEngineOutputEventSubscribersChanged(); });

		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto commandOutputStringFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(commandOutputStringFunction), game);
		commandOutputStringFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(commandOutputStringFunctionContext), game);
		commandOutputStringHook = ModuleMidHook::make(game.toModuleName(), commandOutputStringFunction, commandOutputStringHookFunction);
	}

	~EngineCommandOutputEventImplT()
	{
		std::unique_lock<std::mutex> lock(mDestructionGuard); // block until callbacks/hooks finish executing
		instance = nullptr;
	}

	std::shared_ptr<ObservedEvent<EngineOutputEvent>> getEngineCommandOutputEvent() { return mEngineOutputEvent; }
};



EngineCommandOutputEvent::EngineCommandOutputEvent(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: pimpl = std::make_unique<EngineCommandOutputEventImplT<GameState::Value::Halo1>>(game, dicon); break;
	case GameState::Value::Halo2: pimpl = std::make_unique<EngineCommandOutputEventImplT<GameState::Value::Halo2>>(game, dicon); break;
	case GameState::Value::Halo3: pimpl = std::make_unique<EngineCommandOutputEventImplT<GameState::Value::Halo3>>(game, dicon); break;
	case GameState::Value::Halo3ODST: pimpl = std::make_unique<EngineCommandOutputEventImplT<GameState::Value::Halo3ODST>>(game, dicon); break;
	case GameState::Value::HaloReach: pimpl = std::make_unique<EngineCommandOutputEventImplT<GameState::Value::HaloReach>>(game, dicon); break;
	case GameState::Value::Halo4: pimpl = std::make_unique<EngineCommandOutputEventImplT<GameState::Value::Halo4>>(game, dicon); break;
	}
}

EngineCommandOutputEvent::~EngineCommandOutputEvent() = default;