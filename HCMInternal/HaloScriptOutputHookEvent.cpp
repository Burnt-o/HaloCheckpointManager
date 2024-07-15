#include "pch.h"
#include "HaloScriptOutputHookEvent.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"
#include "MidhookContextInterpreter.h"
#include "RuntimeExceptionHandler.h"
#include "ObservedEventFactory.h"

template <GameState::Value gameT>
class HaloScriptOutputHookEventImplHalo1 : public HaloScriptOutputHookEventImpl
{
private:
	// static stuff for hook
	static inline std::mutex mDestructionGuard{};
	static inline HaloScriptOutputHookEventImplHalo1<gameT>* instance = nullptr;

	// main event we provide
	std::shared_ptr<ObservedEvent<HSOutputEvent>> mHaloScriptOutputEvent;
	std::unique_ptr<ScopedCallback<ActionEvent>> mHaloScriptOutputEventSubscribersChangedCallback;


	// data
	std::shared_ptr<ModuleMidHook> commandOutputStringHook;
	std::shared_ptr<MidhookContextInterpreter> commandOutputStringFunctionContext;
	std::shared_ptr<ModuleMidHook> commandErrorStringHook;
	std::shared_ptr<MidhookContextInterpreter> commandErrorStringFunctionContext;
	std::shared_ptr<ModuleMidHook> helpOutputStringHook;
	std::shared_ptr<MidhookContextInterpreter> helpOutputStringFunctionContext;
	std::shared_ptr<ModuleMidHook> printOutputStringHook;
	std::shared_ptr<ModulePatch> printOutputStringPatch;
	std::shared_ptr<MidhookContextInterpreter> printOutputStringFunctionContext;


	enum class MidhookContextType
	{
		commandOutput,
		commandError,
		helpOutput,
		printOutput,
	};

	std::shared_ptr<MidhookContextInterpreter> getContextByType(MidhookContextType type)
	{
		switch (type)
		{
		case MidhookContextType::commandOutput: return commandOutputStringFunctionContext;
		case MidhookContextType::commandError: return commandErrorStringFunctionContext;
		case MidhookContextType::helpOutput: return helpOutputStringFunctionContext;
		case MidhookContextType::printOutput: return printOutputStringFunctionContext;
		}
	}

	// injected services
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;


	void onHaloScriptOutputEventSubscribersChanged()
	{
		// disable hook if callbacklist is empty (no subscribers), enable otherwise
		bool shouldEnable = mHaloScriptOutputEvent->isEventSubscribed();

		PLOG_DEBUG << "setting HaloScriptOutput hooks state to: " << (shouldEnable ? "true" : "false");
		commandOutputStringHook->setWantsToBeAttached(shouldEnable);
		commandErrorStringHook->setWantsToBeAttached(shouldEnable);
		helpOutputStringHook->setWantsToBeAttached(shouldEnable);
		printOutputStringHook->setWantsToBeAttached(shouldEnable);
		printOutputStringPatch->setWantsToBeAttached(shouldEnable);
	}



	// shared by the hooks since the logic is very similiar
	template<HSOutputType outputType, MidhookContextType contextType, bool checkStringLength>
	static void outputHookFunction(SafetyHookContext& ctx)
	{
		if (!instance)
			return;

		std::unique_lock<std::mutex> lock(mDestructionGuard);
		LOG_ONCE(PLOG_DEBUG << "commandOutputStringHookFunction running");

		try
		{
			enum class param
			{
				pOutputString,
				stringLength
			};
			auto* ctxInterpreter = instance->getContextByType(contextType).get();


			try
			{
				// will be nullptr if there was no output. In which case return early.
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


			std::string out;
			if constexpr (checkStringLength)
			{
				try
				{
					auto* pStringLength = (int*)ctxInterpreter->getParameterRef(ctx, (int)param::stringLength);
					if (IsBadReadPtr(pStringLength, sizeof(int)))
						throw HCMRuntimeException(std::format("Bad read of command string length! at {:X}", (uintptr_t)pStringLength));

					out = std::string(outputChars, *pStringLength);
				}
				catch (HCMRuntimeException ex)
				{
					PLOG_DEBUG << "error parsing stringLength: " << ex.what();
					return;
				}
			}
			else
			{
				out = std::string(outputChars);
			}

			if (out.empty())
			{
				PLOG_DEBUG << "empty output string";
				return;
			}


			PLOG_DEBUG << "Firing engine output event with string: " << out;
			PLOG_DEBUG << "Context type was: " << magic_enum::enum_name(contextType);
			instance->mHaloScriptOutputEvent->fireEvent(out, outputType);
		}
		catch (HCMRuntimeException ex)
		{
			instance->runtimeExceptions->handleMessage(ex);
		}
	}



public:
	HaloScriptOutputHookEventImplHalo1(GameState game, IDIContainer& dicon)
		: runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{

		mHaloScriptOutputEvent = ObservedEventFactory::makeObservedEvent<HSOutputEvent>();
		mHaloScriptOutputEventSubscribersChangedCallback = ObservedEventFactory::getCallbackListChangedCallback(mHaloScriptOutputEvent, [this]() {onHaloScriptOutputEventSubscribersChanged(); });

		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto commandOutputStringFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(commandOutputStringFunction), game);
		commandOutputStringFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(commandOutputStringFunctionContext), game);
		commandOutputStringHook = ModuleMidHook::make(game.toModuleName(), commandOutputStringFunction, outputHookFunction<HSOutputType::Normal, MidhookContextType::commandOutput, false>);

		auto commandErrorStringFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(commandErrorStringFunction), game);
		commandErrorStringFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(commandErrorStringFunctionContext), game);
		commandErrorStringHook = ModuleMidHook::make(game.toModuleName(), commandErrorStringFunction, outputHookFunction<HSOutputType::Error, MidhookContextType::commandError, false>);

		auto helpOutputStringFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(helpOutputStringFunction), game);
		helpOutputStringFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(helpOutputStringFunctionContext), game);
		helpOutputStringHook = ModuleMidHook::make(game.toModuleName(), helpOutputStringFunction, outputHookFunction<HSOutputType::Normal, MidhookContextType::helpOutput, true>);

		auto printOutputStringFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(printOutputStringFunction), game);
		printOutputStringFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(printOutputStringFunctionContext), game);
		printOutputStringHook = ModuleMidHook::make(game.toModuleName(), printOutputStringFunction, outputHookFunction<HSOutputType::Normal, MidhookContextType::printOutput, true>);

		auto printOutputStringPatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(printOutputStringPatchFunction), game);
		auto printOutputStringPatchCode = ptr->getVectorData<byte>(nameof(printOutputStringPatchCode), game);
		printOutputStringPatch = ModulePatch::make(game.toModuleName(), printOutputStringPatchFunction, *printOutputStringPatchCode.get());
	}

	~HaloScriptOutputHookEventImplHalo1()
	{
		std::unique_lock<std::mutex> lock(mDestructionGuard); // block until callbacks/hooks finish executing
		instance = nullptr;
	}

	virtual std::shared_ptr<ObservedEvent<HSOutputEvent>> getHaloScriptOutputEvent() override { return mHaloScriptOutputEvent; }
};


template <GameState::Value gameT>
class HaloScriptOutputHookEventImplGeneric : public HaloScriptOutputHookEventImpl
{
private:
	// static stuff for hook
	static inline std::mutex mDestructionGuard{};
	static inline HaloScriptOutputHookEventImplGeneric<gameT>* instance = nullptr;

	// main event we provide
	std::shared_ptr<ObservedEvent<HSOutputEvent>> mHaloScriptOutputEvent;
	std::unique_ptr<ScopedCallback<ActionEvent>> mHaloScriptOutputEventSubscribersChangedCallback;


	// data
	std::shared_ptr<ModuleMidHook> commandOutputStringHook;
	std::shared_ptr<MidhookContextInterpreter> commandOutputStringFunctionContext;
	std::shared_ptr<ModuleMidHook> commandErrorStringHook;
	std::shared_ptr<MidhookContextInterpreter> commandErrorStringFunctionContext;



	enum class MidhookContextType
	{
		commandOutput,
		commandError,
	};

	std::shared_ptr<MidhookContextInterpreter> getContextByType(MidhookContextType type)
	{
		switch (type)
		{
		case MidhookContextType::commandOutput: return commandOutputStringFunctionContext;
		case MidhookContextType::commandError: return commandErrorStringFunctionContext;

		}
	}

	// injected services
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;


	void onHaloScriptOutputEventSubscribersChanged()
	{
		// disable hook if callbacklist is empty (no subscribers), enable otherwise
		bool shouldEnable = mHaloScriptOutputEvent->isEventSubscribed();

		PLOG_DEBUG << "setting HaloScriptOutput hooks state to: " << (shouldEnable ? "true" : "false");
		commandOutputStringHook->setWantsToBeAttached(shouldEnable);
		commandErrorStringHook->setWantsToBeAttached(shouldEnable);
	}



	// shared by the hooks since the logic is very similiar
	template<HSOutputType outputType, MidhookContextType contextType, bool checkStringLength>
	static void outputHookFunction(SafetyHookContext& ctx)
	{
		if (!instance)
			return;

		std::unique_lock<std::mutex> lock(mDestructionGuard);
		LOG_ONCE(PLOG_DEBUG << "commandOutputStringHookFunction running");

		try
		{
			enum class param
			{
				pOutputString,
				stringLength
			};
			auto* ctxInterpreter = instance->getContextByType(contextType).get();


			try
			{
				// will be nullptr if there was no output. In which case return early.
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


			std::string out;
			if constexpr (checkStringLength)
			{
				try
				{
					auto* pStringLength = (int*)ctxInterpreter->getParameterRef(ctx, (int)param::stringLength);
					if (IsBadReadPtr(pStringLength, sizeof(int)))
						throw HCMRuntimeException(std::format("Bad read of command string length! at {:X}", (uintptr_t)pStringLength));

					out = std::string(outputChars, *pStringLength);
				}
				catch (HCMRuntimeException ex)
				{
					PLOG_DEBUG << "error parsing stringLength: " << ex.what();
					return;
				}
			}
			else
			{
				out = std::string(outputChars);
			}

			if (out.empty())
			{
				PLOG_DEBUG << "empty output string";
				return;
			}


			PLOG_DEBUG << "Firing engine output event with string: " << out;
			PLOG_DEBUG << "Context type was: " << magic_enum::enum_name(contextType);
			instance->mHaloScriptOutputEvent->fireEvent(out, outputType);
		}
		catch (HCMRuntimeException ex)
		{
			instance->runtimeExceptions->handleMessage(ex);
		}
	}



public:
	HaloScriptOutputHookEventImplGeneric(GameState game, IDIContainer& dicon)
		: runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{

		mHaloScriptOutputEvent = ObservedEventFactory::makeObservedEvent<HSOutputEvent>();
		mHaloScriptOutputEventSubscribersChangedCallback = ObservedEventFactory::getCallbackListChangedCallback(mHaloScriptOutputEvent, [this]() {onHaloScriptOutputEventSubscribersChanged(); });

		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto commandOutputStringFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(commandOutputStringFunction), game);
		commandOutputStringFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(commandOutputStringFunctionContext), game);
		commandOutputStringHook = ModuleMidHook::make(game.toModuleName(), commandOutputStringFunction, outputHookFunction<HSOutputType::Normal, MidhookContextType::commandOutput, false>);

		auto commandErrorStringFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(commandErrorStringFunction), game);
		commandErrorStringFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(commandErrorStringFunctionContext), game);
		commandErrorStringHook = ModuleMidHook::make(game.toModuleName(), commandErrorStringFunction, outputHookFunction<HSOutputType::Error, MidhookContextType::commandError, false>);
	}

	~HaloScriptOutputHookEventImplGeneric()
	{
		std::unique_lock<std::mutex> lock(mDestructionGuard); // block until callbacks/hooks finish executing
		instance = nullptr;
	}

	virtual std::shared_ptr<ObservedEvent<HSOutputEvent>> getHaloScriptOutputEvent() override { return mHaloScriptOutputEvent; }
};


HaloScriptOutputHookEvent::HaloScriptOutputHookEvent(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: pimpl = std::make_unique<HaloScriptOutputHookEventImplHalo1<GameState::Value::Halo1>>(game, dicon); break;
	case GameState::Value::Halo2: pimpl = std::make_unique<HaloScriptOutputHookEventImplGeneric<GameState::Value::Halo2>>(game, dicon); break;
	case GameState::Value::Halo3: pimpl = std::make_unique<HaloScriptOutputHookEventImplGeneric<GameState::Value::Halo3>>(game, dicon); break;
	case GameState::Value::Halo3ODST: pimpl = std::make_unique<HaloScriptOutputHookEventImplGeneric<GameState::Value::Halo3ODST>>(game, dicon); break;
	case GameState::Value::HaloReach: pimpl = std::make_unique<HaloScriptOutputHookEventImplGeneric<GameState::Value::HaloReach>>(game, dicon); break;
	case GameState::Value::Halo4: pimpl = std::make_unique<HaloScriptOutputHookEventImplGeneric<GameState::Value::Halo4>>(game, dicon); break;
	default: throw HCMInitException("not impl yet");
	}
}

HaloScriptOutputHookEvent::~HaloScriptOutputHookEvent() = default;