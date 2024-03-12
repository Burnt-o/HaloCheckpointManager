#include "pch.h"
#include "SkullToggler.h"
#include "SkullEnum.h"
#include "BitBoolPointer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"

template <GameState::Value gameT>
class SkullTogglerImpl : public ISkullTogglerImpl
{
private:
	// main store requested by GUI. protected by atomic bool


	// cache stuff
	bool cacheValid = false;
	std::map<SkullEnum, std::shared_ptr<MultilevelPointer>> skullDataPointers;

	// event callbacks
	ScopedCallback<eventpp::CallbackList<void (GameState)>> updateSkullBitBoolCollectionEventCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// injected services
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;


	// called by skull toggle gui before it wants to use the skullBitBoolCollection
	void onUpdateSkullBitBoolCollectionEvent(GameState game)
	{
		if (game.operator GameState::Value() != gameT) return; // not our game, not our problem

		if (cacheValid == true)
		{
			return; // no work to do
		}
		else // update cache!
		{
			try
			{
				lockOrThrow(settingsWeak, settings);
				
				if (settings->skullBitBoolCollectionInUse) // wait atomic bool
				{
					settings->skullBitBoolCollectionInUse.wait(true);
				}

				ScopedAtomicBool lock(settings->skullBitBoolCollectionInUse); // lock atomic bool

				settings->skullBitBoolCollection.clear(); // clear cache

				for (auto& [skullEnumKey, dataPointer] : skullDataPointers) // loop thru our valid data pointers and update cache
				{
					uintptr_t skullPointer;
					if (dataPointer->resolve(&skullPointer))
						settings->skullBitBoolCollection.insert({ skullEnumKey, BitBoolPointer(skullPointer, dataPointer->getBitOffset()) });
					else
						PLOG_ERROR << "error resolving skull pointer " << magic_enum::enum_name(skullEnumKey) << ": " << MultilevelPointer::GetLastError();
				}

				cacheValid = true;
			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
			
		}

	}

	// event to fire on mcc state change (set flag saying we need to update cache)
	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing SkullTogglerImpl caches";
		cacheValid = false;
	}


public:
	SkullTogglerImpl(GameState gameImpl, IDIContainer& dicon)
		: 
		updateSkullBitBoolCollectionEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->updateSkullBitBoolCollectionEvent, [this](GameState g) { onUpdateSkullBitBoolCollectionEvent(g); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); })
	{
		std::map < SkullEnum, std::set<GameState::Value>> skullEnumToSupportedGames =
		{
			{SkullEnum::Anger, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST} },
			{SkullEnum::Acrophobia, {GameState::Value::Halo1, GameState::Value::Halo2, GameState::Value::Halo3, GameState::Value::Halo3ODST, GameState::Value::HaloReach, GameState::Value::Halo4}}
		};

		auto pointerManager = dicon.Resolve<PointerManager>().lock();
		for (auto& [skullEnum, supportedGameCollection] : skullEnumToSupportedGames)
		{
			if (supportedGameCollection.contains(gameImpl))
			{
				std::string pointerName = std::format("skull{}Pointer", magic_enum::enum_name(skullEnum));
				try
				{
					auto pointer = pointerManager->getData<std::shared_ptr<MultilevelPointer>>(pointerName, gameImpl);
					skullDataPointers.emplace(skullEnum, pointer);
				}
				catch(HCMInitException ex)
				{
					PLOG_ERROR << "skull toggler could not resolve " << pointerName << " for game " << gameImpl.toString() << ": " << ex.what();
				}
			}

		}

		if (skullDataPointers.empty()) throw HCMInitException("Could not resolve any data pointers for skulls!");
	}
};







SkullToggler::SkullToggler(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<SkullTogglerImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<SkullTogglerImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;


	default:
		throw HCMInitException("not impl yet");
	}
}

SkullToggler::~SkullToggler() = default;