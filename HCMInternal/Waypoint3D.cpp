#include "pch.h"
#include "Waypoint3D.h"
#include "PointerManager.h"
#include "WaypointList.h"
#include "Render3DEventProvider.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "Render2DHelpers.h"
#include "MeasurePlayerDistanceToObject.h"


// TODO: turning on display2dinfo fucks with the font



template<GameState::Value mGame>
class Waypoint3DImpl : public Waypoint3DUntemplated {

private:
	// callbacks
	std::unique_ptr<ScopedCallback<Render3DEvent>> mRenderEventCallback;
	std::atomic_bool renderingMutex = false;
	ScopedCallback<ToggleEvent> mWaypoint3DToggleEventCallback;
	ScopedCallback<eventpp::CallbackList<void(WaypointList&)>> mWaypointListChangedCallback;


	// injectedservices
	std::weak_ptr<Render3DEventProvider> render3DEventProviderWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::optional<std::weak_ptr<MeasurePlayerDistanceToObject>> measurePlayerDistanceToObjectOptionalWeak;

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
			if (renderingMutex)
			{
				renderingMutex.wait(true);
			}
			mRenderEventCallback.reset();
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
				mRenderEventCallback = std::make_unique<ScopedCallback<Render3DEvent>>(render3DEventProvider->render3DEvent, [this](IRenderer3D* n) {onRenderEvent(n); });

			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
			
		}
		else
		{
			if (renderingMutex)
			{
				renderingMutex.wait(true);
			}
			mRenderEventCallback.reset();
		}

	}

	// new frame, render
	void onRenderEvent(IRenderer3D* renderer)
	{
		ScopedAtomicBool lockRender(renderingMutex);
		LOG_ONCE(PLOG_VERBOSE << "onRenderEvent");
		uint32_t textColor = 0xFF00FF00; // green. TODO: let user set desired colour.
		constexpr int distancePrecision = 3; // TODO: let user set.
		constexpr float fontBaseScale = 1.f; // TODO: let user set.


		std::optional<std::shared_ptr<MeasurePlayerDistanceToObject>> measurePlayerDistanceToObjectLocked;

		if (measurePlayerDistanceToObjectOptionalWeak.has_value())
		{
			measurePlayerDistanceToObjectLocked = measurePlayerDistanceToObjectOptionalWeak.value().lock();


			if (!measurePlayerDistanceToObjectLocked.value())
			{
				measurePlayerDistanceToObjectLocked = std::nullopt;
			}

		}






		
		std::scoped_lock<std::mutex> lockWaypointList(waypointListInternalMutex);
		for (auto& waypoint : waypointListInternal.list)
		{
			auto screenPosition = renderer->worldPointToScreenPosition(waypoint.position);
			if (screenPosition.z < 0 || screenPosition.z > 1) continue; // clipped


			if (waypoint.label.has_value())
			{
				LOG_ONCE(PLOG_DEBUG << "rendering waypoint label");
				float fontDistanceScale = fontBaseScale * Render2D::scaleTextDistance(renderer->cameraDistanceToWorldPoint(waypoint.position));

				// draw main label text
				RECTF labelTextRect = Render2D::drawCenteredOutlinedText(waypoint.label.value(), {screenPosition.x, screenPosition.y}, textColor, fontDistanceScale);

				// also draw distance measurement, rendering it BELOW the label text.
				if (waypoint.showDistance && measurePlayerDistanceToObjectLocked.has_value())
				{
					LOG_ONCE(PLOG_DEBUG << "also rendering distance measure");
					auto distance = measurePlayerDistanceToObjectLocked.value()->measure(waypoint.position);
					if (distance.has_value())
					{
						// text drawn BELOW label text with (fontscale * 3px) vertical padding. 
						Render2D::drawCenteredOutlinedText(
							std::format("{} units", to_string_with_precision(distance.value(), distancePrecision)), 
							{ screenPosition.x, labelTextRect.bottom + (fontDistanceScale * 3.f) }, 
							textColor, 
							fontDistanceScale);
					}
					else
					{
						Render2D::drawCenteredOutlinedText(
							"Measurement Error", 
							{ screenPosition.x + (labelTextRect.right - labelTextRect.left), labelTextRect.bottom + 5.f }, 
							textColor,
							fontDistanceScale);
					}

				}
			}
			else
			{
				LOG_ONCE(PLOG_DEBUG << "rendering distance measure only");
				// Only draw distance measurement. Draws distance text directly on screen position since there's no label to place it below.

				if (waypoint.showDistance && measurePlayerDistanceToObjectLocked.has_value())
				{
					float fontDistanceScale = fontBaseScale * Render2D::scaleTextDistance(renderer->cameraDistanceToWorldPoint(waypoint.position));

					auto distance = measurePlayerDistanceToObjectLocked.value()->measure(waypoint.position);
					if (distance.has_value())
					{
						Render2D::drawCenteredOutlinedText(
							std::format("{} units", to_string_with_precision(distance.value(), distancePrecision)), 
							{ screenPosition.x, screenPosition.y }, 
							textColor, 
							fontDistanceScale);
					}
					else
					{
						Render2D::drawCenteredOutlinedText(
							"Measurement Error", 
							{ screenPosition.x, screenPosition.y }, 
							textColor, 
							fontDistanceScale);
					}

				}
			}
			
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
		try
		{
			measurePlayerDistanceToObjectOptionalWeak = resolveDependentCheat(MeasurePlayerDistanceToObject);
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "failed to resolve measurePlayerDistanceToObject service: " << ex.what();
		}
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