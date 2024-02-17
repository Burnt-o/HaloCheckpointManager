#include "pch.h"
#include "Waypoint3D.h"
#include "PointerManager.h"
#include "WaypointList.h"
#include "Render3DEventProvider.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"


template<GameState::Value mGame>
class Waypoint3DImpl : public Waypoint3DUntemplated {

private:
	// callbacks
	std::optional<ScopedCallback<Render3DEvent>> mRenderEventCallback;
	ScopedCallback<ToggleEvent> mWaypoint3DToggleEventCallback;
	ScopedCallback<eventpp::CallbackList<void(WaypointList&)>> mWaypointListChangedCallback;


	// injectedservices
	std::weak_ptr<Render3DEventProvider> render3DEventProviderWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	WaypointList waypointListInternal; // copy of the setting for thread safety.
	std::mutex waypointListInternalMutex; // locked while above is updated. locked by onRenderEvent while in use.

	// when user adds, removes, or edits waypoint list
	void onWaypointListChanged(WaypointList& waypointListSetting)
	{
		std::scoped_lock<std::mutex> lock(waypointListInternalMutex);
		waypointListInternal = waypointListSetting;
	}

	// when user toggles whether 3d waypoints are enabled
	void onToggleChange(bool& newValue)
	{
		lockOrThrow(mccStateHookWeak, mccStateHook);
		if (mccStateHook->isGameCurrentlyPlaying(mGame) == false)
		{
			mRenderEventCallback = std::nullopt;
			return;
		}

		std::scoped_lock<std::mutex> lock(waypointListInternalMutex);
		// subscribe to 3d render event if enabled, unsubscribe otherwise
		if (newValue)
		{
			try
			{
				lockOrThrow(settingsWeak, settings);
				waypointListInternal = settings->waypoint3DList->GetValue();


				lockOrThrow(render3DEventProviderWeak, render3DEventProvider);
				mRenderEventCallback = std::move(ScopedCallback<Render3DEvent>(render3DEventProvider->render3DEvent, [this](IRenderer3D* n) {onRenderEvent(n); }));

			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
			
		}
		else
		{
			mRenderEventCallback = std::nullopt;
		}

	}

	// new frame, render
	void onRenderEvent(IRenderer3D* renderer)
	{
		LOG_ONCE(PLOG_VERBOSE << "onRenderEvent");
		constexpr SimpleMath::Vector4 textColor{0, 1, 0, 1}; // green

		std::scoped_lock<std::mutex> lock(waypointListInternalMutex);

		// for initial testing, we'll just draw "hello world" at each waypoint.
		for (auto& waypoint : waypointListInternal.list)
		{
			renderer->draw3DText("Hello World!", waypoint.position, textColor);
		}

	}

public:
	Waypoint3DImpl(GameState game, IDIContainer& dicon) :
		render3DEventProviderWeak(resolveDependentCheat(Render3DEventProvider)),
		mWaypoint3DToggleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->waypoint3DToggle->valueChangedEvent, [this](bool& n) {onToggleChange(n); }),
		mWaypointListChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->waypoint3DList->valueChangedEvent, [this](WaypointList& n) {onWaypointListChanged(n); }),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>())
	{
		// TODO: add callback to mccstate change, removing render callback when it happens.
	}
};


Waypoint3D::Waypoint3D(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<Waypoint3DImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<Waypoint3DImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<Waypoint3DImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<Waypoint3DImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<Waypoint3DImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<Waypoint3DImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

Waypoint3D::~Waypoint3D()
{
		PLOG_VERBOSE << "~" << getName();
}