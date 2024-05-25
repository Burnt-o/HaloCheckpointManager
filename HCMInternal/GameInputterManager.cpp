#include "pch.h"
#include "GameInputterManager.h"
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"
#include "DynamicStructFactory.h"
#include "GameTickEventHook.h"
#include "RuntimeExceptionHandler.h"
#include "IMakeOrGetCheat.h"

class ICanSetInput
{
public:
	virtual ~ICanSetInput() = default;
	virtual void setQueuedInput(const GameInputEnum& input, int ticksToPress) = 0;
	virtual void setCurrentInput(const GameInputEnum& input, bool value) = 0;
	virtual void advanceInputQueue(int ticksToAdvance) = 0;
	virtual void clearInputQueue() = 0;
};

class ScopedGameInputterImpl : public IScopedGameInputter
{
private:
	std::unique_ptr<ScopedServiceRequest> mScopedInputOverride;
	std::shared_ptr<ICanSetInput> mGameInputManagerImpl;
public:
	ScopedGameInputterImpl(std::unique_ptr<ScopedServiceRequest> scopedInputOverride, std::shared_ptr<ICanSetInput> gameInputManagerImpl)
		: mScopedInputOverride(std::move(scopedInputOverride)), mGameInputManagerImpl(gameInputManagerImpl)
	{}
	virtual void setQueuedInput(const GameInputEnum& input, int ticksToPress) override
	{
		mGameInputManagerImpl->setQueuedInput(input, ticksToPress);
	}

	virtual void setCurrentInput(const GameInputEnum& input, bool value) override
	{
		mGameInputManagerImpl->setCurrentInput(input, value);
	}

	virtual void advanceInputQueue(int ticksToAdvance) override
	{
		mGameInputManagerImpl->advanceInputQueue(ticksToAdvance);
	}

	virtual void clearInputQueue() override
	{
		mGameInputManagerImpl->clearInputQueue();
	}
};


class GameInputterManagerImpl : public GenericScopedServiceProvider, public ICanSetInput
{
private:
	
	// data
	std::shared_ptr<ModulePatch> overrideGameInputPatch;
	std::shared_ptr<MultilevelPointer> gameInputFieldPointer;
	std::shared_ptr<DynamicStruct<GameInputEnum>> gameInputFieldStruct;
	typedef int ticksLeftOfInput;
	std::vector<std::pair<GameInputEnum, ticksLeftOfInput>> gameInputQueue;

	// callbacks
	std::unique_ptr<ScopedCallback<eventpp::CallbackList<void(int)>>> mGameTickEventCallback;

	// injected services
	std::weak_ptr<GameTickEventHook> gameTickEventHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// called when ScopedGameInputter created or destructed
	virtual void updateService() override
	{
		try
		{
			if (this->serviceIsRequested())
			{
				overrideGameInputPatch->setWantsToBeAttached(true);
				lockOrThrow(gameTickEventHookWeak, gameTickEventHook);
				mGameTickEventCallback = std::make_unique<ScopedCallback<eventpp::CallbackList<void(int)>>>(gameTickEventHook->getGameTickEvent(), [this](int i) {onGameTickEvent(); });
			}
			else
			{
				overrideGameInputPatch->setWantsToBeAttached(false);
				mGameTickEventCallback.reset();
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}

	void writeInputThisTick(const GameInputEnum& input, bool value)
	{
		PLOG_DEBUG << "writeInputThisTick";
		gameInputFieldPointer->resolve(&gameInputFieldStruct->currentBaseAddress);
		auto* inputPtr = gameInputFieldStruct->field<byte>(input);

		PLOG_DEBUG << "writing input to ptr: " << std::hex << (uintptr_t)inputPtr;
		PLOG_DEBUG << "input: " << magic_enum::enum_name(input);
		PLOG_DEBUG << "value: " << (value ? "true" : "false");

		*inputPtr = (value ? 0xFF : 0x00);
	}


	void onGameTickEvent()
	{
		if (gameInputQueue.empty() == false)
		{
			// set inputs to 0x00
			for (auto& queuedInput : gameInputQueue)
			{
				if (queuedInput.second < 1)
					writeInputThisTick(queuedInput.first, false);

			}

			// remove inputs from the queue that have been pressed for enough ticks (ticksLeftOfInput < 1)
			std::erase_if(gameInputQueue, [](const auto& queuedInput) { return queuedInput.second < 1; });

			// set inputs to 0xFF
			for (auto& queuedInput : gameInputQueue)
			{
				writeInputThisTick(queuedInput.first, true);
				queuedInput.second -= 1; // decrement the ticksLeftOfInput counter

			}



		}
	}

public:

	virtual void setQueuedInput(const GameInputEnum& input, int ticksToPress) override
	{
		gameInputQueue.push_back(std::make_pair(input, ticksToPress));
	}

	virtual void setCurrentInput(const GameInputEnum& input, bool value) override
	{
		writeInputThisTick(input, value);
	}

	virtual void advanceInputQueue(int ticksToAdvance) override
	{
		for (int i = 0; i < ticksToAdvance; i++)
		{
			onGameTickEvent();
		}
	}

	virtual void clearInputQueue() override
	{
		for (auto& queuedInput : gameInputQueue)
		{
			writeInputThisTick(queuedInput.first, false);
		}

		gameInputQueue.clear();
	}

	GameInputterManagerImpl(GameState game, IDIContainer& dicon)
		: gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook)),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>().lock())
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		gameInputFieldStruct = DynamicStructFactory::make<GameInputEnum>(ptr, game);
		gameInputFieldPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameInputFieldPointer), game);

		auto overrideGameInputFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(overrideGameInputFunction), game);
		auto overideGameInputPatchCode = ptr->getVectorData<byte>(nameof(overideGameInputPatchCode), game);
		overrideGameInputPatch = ModulePatch::make(game.toModuleName(), overrideGameInputFunction, *overideGameInputPatchCode.get());
		
	}
};




std::unique_ptr<IScopedGameInputter> GameInputterManager::getScopedGameInputter(std::string callerID)
{
	return std::make_unique<ScopedGameInputterImpl>(pimpl->makeRequest(callerID), pimpl);
}


GameInputterManager::GameInputterManager(GameState gameImpl, IDIContainer& dicon)
{
	if (gameImpl.operator GameState::Value() == GameState::Value::Halo1)
	{
		pimpl = std::make_shared<GameInputterManagerImpl>(gameImpl, dicon);
	}
	else
	{
		throw HCMInitException("Game not supported");
	}
}

GameInputterManager::~GameInputterManager() = default;