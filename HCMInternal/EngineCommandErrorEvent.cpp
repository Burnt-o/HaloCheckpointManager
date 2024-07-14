#include "pch.h"
#include "EngineCommandErrorEvent.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"
#include "MidhookContextInterpreter.h"
#include "RuntimeExceptionHandler.h"
#include "ObservedEventFactory.h"

template <GameState::Value gameT>
class EngineCommandErrorEventImplT : public EngineCommandErrorEventImpl
{
private:
	// static stuff for hook
	static inline std::mutex mDestructionGuard{};
	static inline EngineCommandErrorEventImplT<gameT>* instance = nullptr;

	// main event we provide
	std::shared_ptr<ObservedEvent<EngineErrorEvent>> mEngineErrorEvent;
	std::unique_ptr<ScopedCallback<ActionEvent>> mEngineErrorEventSubscribersChangedCallback;


	// data
	std::shared_ptr<ModuleMidHook> commandErrorStringHook;
	std::shared_ptr<MidhookContextInterpreter> commandErrorStringFunctionContext;

	// injected services
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;


	void onEngineErrorEventSubscribersChanged()
	{
		// disable hook if callbacklist is empty (no subscribers), enable otherwise
		PLOG_DEBUG << "setting commandErrorStringHook attach state to: " << (mEngineErrorEvent->isEventSubscribed() ? "true" : "false");
		commandErrorStringHook->setWantsToBeAttached(mEngineErrorEvent->isEventSubscribed());
	}



	// for error messages
	static void commandErrorStringHookFunction(SafetyHookContext& ctx)
	{
		if (!instance)
			return;

		std::unique_lock<std::mutex> lock(mDestructionGuard);
		LOG_ONCE(PLOG_DEBUG << "commandErrorStringHookFunction running");

		try
		{
			enum class param
			{
				pErrorString
			};
			auto* ctxInterpreter = instance->commandErrorStringFunctionContext.get();
			
			try
			{
				// will be nullptr if there was no error. In which case return early.
				uintptr_t errorCharsPointer = (uintptr_t)ctxInterpreter->getParameterRef(ctx, (int)param::pErrorString);
				if (errorCharsPointer == 0)
				{
					PLOG_DEBUG << "no error";
					return;
				}

				PLOG_DEBUG << "errorCharsPointer: " << std::hex << errorCharsPointer;
			}
			catch (HCMRuntimeException ex)
			{
				PLOG_DEBUG << "error parsing errorCharsPointer: " << ex.what();
				return;
			}




			const char* errorChars = (const char*)ctxInterpreter->getParameterRef(ctx, (int)param::pErrorString);
			if (IsBadReadPtr(errorChars, 4))
				throw HCMRuntimeException(std::format("Bad read of command string error characters! at {:X}", (uintptr_t)errorChars));



			auto out = std::string(errorChars);
			PLOG_DEBUG << "firing EngineErrorEvent with string: " << out;
			instance->mEngineErrorEvent->fireEvent(out);
		}
		catch (HCMRuntimeException ex)
		{
			instance->runtimeExceptions->handleMessage(ex);
		}
	}



public:
	EngineCommandErrorEventImplT(GameState game, IDIContainer& dicon)
		: runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{

		mEngineErrorEvent = ObservedEventFactory::makeObservedEvent<EngineErrorEvent>();
		mEngineErrorEventSubscribersChangedCallback = ObservedEventFactory::getCallbackListChangedCallback(mEngineErrorEvent, [this]() {onEngineErrorEventSubscribersChanged(); });

		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto commandErrorStringFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(commandErrorStringFunction), game);
		commandErrorStringFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(commandErrorStringFunctionContext), game);
		commandErrorStringHook = ModuleMidHook::make(game.toModuleName(), commandErrorStringFunction, commandErrorStringHookFunction);
	}

	~EngineCommandErrorEventImplT()
	{
		std::unique_lock<std::mutex> lock(mDestructionGuard); // block until callbacks/hooks finish executing
		instance = nullptr;
	}

	std::shared_ptr<ObservedEvent<EngineErrorEvent>> getEngineCommandErrorEvent() { return mEngineErrorEvent; }
};



EngineCommandErrorEvent::EngineCommandErrorEvent(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: pimpl = std::make_unique<EngineCommandErrorEventImplT<GameState::Value::Halo1>>(game, dicon); break;
	case GameState::Value::Halo2: pimpl = std::make_unique<EngineCommandErrorEventImplT<GameState::Value::Halo2>>(game, dicon); break;
	case GameState::Value::Halo3: pimpl = std::make_unique<EngineCommandErrorEventImplT<GameState::Value::Halo3>>(game, dicon); break;
	case GameState::Value::Halo3ODST: pimpl = std::make_unique<EngineCommandErrorEventImplT<GameState::Value::Halo3ODST>>(game, dicon); break;
	case GameState::Value::HaloReach: pimpl = std::make_unique<EngineCommandErrorEventImplT<GameState::Value::HaloReach>>(game, dicon); break;
	case GameState::Value::Halo4: pimpl = std::make_unique<EngineCommandErrorEventImplT<GameState::Value::Halo4>>(game, dicon); break;
	}
}

EngineCommandErrorEvent::~EngineCommandErrorEvent() = default;