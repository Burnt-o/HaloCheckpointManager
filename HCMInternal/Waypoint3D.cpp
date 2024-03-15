#include "pch.h"
#include "Waypoint3D.h"
#include "PointerDataStore.h"
#include "WaypointList.h"
#include "Render3DEventProvider.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "MeasurePlayerDistanceToObject.h"
#include "RenderTextHelper.h"
#include "ModalDialogRenderer.h"
#include "GetPlayerDatum.h"
#include "GetObjectPhysics.h"
#include "ModalDialogFactory.h"

// TODO: scale font/sprite size by screen size (actually should I?)



template<GameState::Value mGame>
class Waypoint3DImpl : public Waypoint3DUntemplated {

private:
	// callbacks
	std::unique_ptr<ScopedCallback<Render3DEvent>> mRenderEventCallback;
	std::atomic_bool renderingMutex = false;
	ScopedCallback<ToggleEvent> mWaypoint3DToggleEventCallback;
	ScopedCallback<WaypointAndListEvent> mEditWaypointEventCallback;
	ScopedCallback<WaypointAndListEvent> mDeleteWaypointEventCallback;
	ScopedCallback<WaypointListEvent> mAddWaypointEventCallback;



	// injectedservices
	std::weak_ptr<Render3DEventProvider> render3DEventProviderWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<ModalDialogRenderer> modalDialogsWeak;
	std::optional<std::weak_ptr<GetPlayerDatum>> getPlayerDatumOptionalWeak;
	std::optional<std::weak_ptr<GetObjectPhysics>> getObjectPhysicsOptionalWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::optional<std::weak_ptr<MeasurePlayerDistanceToObject>> measurePlayerDistanceToObjectOptionalWeak;

	// for the "fill with player position" button on the waypoint add/edit dialog
	std::optional<SimpleMath::Vector3> getPlayerPositionForDialog()
	{
		
		if (getPlayerDatumOptionalWeak.has_value() == false || getObjectPhysicsOptionalWeak.has_value() == false)
			return std::nullopt;

		try
		{
			lockOrThrow(getPlayerDatumOptionalWeak.value(), getPlayerDatum);
			lockOrThrow(getObjectPhysicsOptionalWeak.value(), getObjectPhysics);

			auto playerDatum = getPlayerDatum->getPlayerDatum();
			if (playerDatum.isNull()) throw HCMRuntimeException("null player datum");

			auto playerPosition = getObjectPhysics->getObjectPosition(playerDatum);
			return *playerPosition;
		}
		catch (HCMRuntimeException ex)
		{
			ex.prepend("Couldn't parse player position for waypoint dialog: ");
				runtimeExceptions->handleMessage(ex);
			return std::nullopt;
		}
	}
	
	void onEditWaypointEvent(Waypoint& wp, WaypointList& wplist) 
	{
		// call modal dialog, passing original waypoint. Dialog will make a copy of this waypoint which the user will edit.
		// Dialog will return a waypoint if user proceeds, or std::nullopt if they cancel.
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;


			lockOrThrow(modalDialogsWeak, modalDialogs);
			auto maybeNewWaypoint = modalDialogs->showReturningDialog(ModalDialogFactory::makeEditOrAddWaypointDialog(runtimeExceptions, "Edit waypoint", wp, getPlayerPositionForDialog(), measurePlayerDistanceToObjectOptionalWeak.has_value()));
			
			if (maybeNewWaypoint.has_value())
			{
				if (wplist.listInUse)
				{
					wplist.listInUse.wait(true);
				}
				ScopedAtomicBool lock(wplist.listInUse);
				wp = maybeNewWaypoint.value(); // replace original wp by ref
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onDeleteWaypointEvent(Waypoint& wp, WaypointList& wplist) 
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

			if (wplist.listInUse)
			{
				wplist.listInUse.wait(true);
			}
			ScopedAtomicBool lock(wplist.listInUse);

			// delete the waypoint from the list if the pointer matches
			wplist.list.erase(
				std::remove_if(wplist.list.begin(), wplist.list.end(),
					[&wp](const Waypoint& wpToCheck) { return &wpToCheck == &wp; }),
				wplist.list.end());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
		
	}

	void onAddWaypointEvent(WaypointList& wplist) 
	{
		// call modal dialog, passing a default waypoint. 
		// Dialog will return a waypoint if user proceeds, or std::nullopt if they cancel.
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

			auto defaultPosition = SimpleMath::Vector3{ 0,0,0 };
			try
			{
				if (getPlayerDatumOptionalWeak.has_value() && getObjectPhysicsOptionalWeak.has_value())
				{
					lockOrThrow(getPlayerDatumOptionalWeak.value(), getPlayerDatum);
					lockOrThrow(getObjectPhysicsOptionalWeak.value(), getObjectPhysics);
					defaultPosition = *getObjectPhysics->getObjectPosition(getPlayerDatum->getPlayerDatum());
				}
				PLOG_ERROR << "error getting player position for new waypoint, continuing with 0,0,0";
			}
			catch (HCMRuntimeException ex)
			{
				ex.prepend("error getting player position for new waypoint, continuing with 0,0,0");
				PLOG_ERROR << ex.what();
			}


			Waypoint defaultWaypoint = Waypoint(defaultPosition);

			lockOrThrow(modalDialogsWeak, modalDialogs);
			auto maybeNewWaypoint = modalDialogs->showReturningDialog(ModalDialogFactory::makeEditOrAddWaypointDialog(runtimeExceptions, "Add waypoint", defaultWaypoint, getPlayerPositionForDialog(), measurePlayerDistanceToObjectOptionalWeak.has_value()));

			if (maybeNewWaypoint.has_value())
			{
				if (wplist.listInUse)
				{
					wplist.listInUse.wait(true);
				}
				ScopedAtomicBool lock(wplist.listInUse);
				wplist.list.emplace_back(maybeNewWaypoint.value()); // add new waypoint to end of list
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
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


		// subscribe to 3d render event if enabled, unsubscribe otherwise
		if (newValue)
		{
			try
			{

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
		try // some renderer funcs (sprite drawing) can throw HCMRuntime exceptions
		{
	
			ScopedAtomicBool lockRender(renderingMutex);
			LOG_ONCE(PLOG_VERBOSE << "onRenderEvent");
			constexpr int distancePrecision = 3; // TODO: let user set.



			std::optional<std::shared_ptr<MeasurePlayerDistanceToObject>> measurePlayerDistanceToObjectLocked;

			if (measurePlayerDistanceToObjectOptionalWeak.has_value())
			{
				measurePlayerDistanceToObjectLocked = measurePlayerDistanceToObjectOptionalWeak.value().lock();


				if (!measurePlayerDistanceToObjectLocked.value())
				{
					measurePlayerDistanceToObjectLocked = std::nullopt;
				}

			}




			lockOrThrow(settingsWeak, settings);
			if (settings->waypoint3DList->GetValue().listInUse)
			{
				LOG_ONCE(PLOG_DEBUG << "Waypoint3D::onRenderEvent waiting for waypoint list to no longer be locked");
				settings->waypoint3DList->GetValue().listInUse.wait(true);
			}

			ScopedAtomicBool lock(settings->waypoint3DList->GetValue().listInUse);
			LOG_ONCE(PLOG_DEBUG << "Waypoint3D::onRenderEvent locking waypoint list");

			auto& lockedWaypointList = settings->waypoint3DList->GetValue().list;

#ifdef HCM_DEBUG
			if (GetKeyState('7') & 0x8000)
			{
				PLOG_DEBUG << "lockedWaypointList.size(): " << lockedWaypointList.size();
			}
#endif

		
			for (auto& waypoint : lockedWaypointList)
			{
				if (waypoint.waypointEnabled == false) continue;

				auto screenPosition = renderer->worldPointToScreenPosition(waypoint.position);
				if (screenPosition.z < 0 || screenPosition.z > 1) continue; // clipped

				// test if filtered by render range
				std::optional<float> distanceToCamera = std::nullopt; // optional so we can potentially reuse it later in distance measure text
				if (measurePlayerDistanceToObjectLocked.has_value() && settings->waypoint3DRenderRangeToggle->GetValue())
				{
					distanceToCamera = measurePlayerDistanceToObjectLocked.value()->measure(waypoint.position);
					if (distanceToCamera.value() > settings->waypoint3DRenderRangeInput->GetValue()) continue; // beyond render range
				}

				float renderVerticalOffset = 0.f;
				constexpr float verticalpadding = 10.f;

				// render sprite
				if (waypoint.showSprite)
				{
					LOG_ONCE(PLOG_DEBUG << "rendering waypoint sprite");

					auto& spriteScale = waypoint.spriteScaleUseGlobal ? settings->waypoint3DGlobalSpriteScale->GetValue() : waypoint.spriteScale;
					auto& spriteColor = waypoint.spriteColorUseGlobal ? settings->waypoint3DGlobalSpriteColor->GetValue() : waypoint.spriteColor;

					auto drawnRect = renderer->drawCenteredSprite(110, { screenPosition.x, screenPosition.y }, spriteScale, spriteColor);
					
					renderVerticalOffset += (((drawnRect.bottom - drawnRect.top) / 2.f) + (verticalpadding * spriteScale));
				}


				//  render label
				if (waypoint.showLabel && waypoint.label.empty() == false)
				{
					LOG_ONCE(PLOG_DEBUG << "rendering waypoint label");

					auto& labelScale = waypoint.labelScaleUseGlobal ? settings->waypoint3DGlobalLabelScale->GetValue() : waypoint.labelScale;
					auto& labelColor = waypoint.labelColorUseGlobal ? settings->waypoint3DGlobalLabelColor->GetValue() : waypoint.labelColor;

#ifdef HCM_DEBUG
					if (GetKeyState('6') & 0x8000)
					{
						PLOG_DEBUG << "waypoint.labelColorUseGlobal: " << (waypoint.labelColorUseGlobal ? "true" : "false");
					}
#endif

					float labelfontDistanceScale = labelScale * RenderTextHelper::scaleTextDistance(renderer->cameraDistanceToWorldPoint(waypoint.position));

					// draw main label text
					auto drawnRect = RenderTextHelper::drawCenteredOutlinedText(
						waypoint.label, 
						{screenPosition.x, screenPosition.y + renderVerticalOffset}, 
						ImGui::ColorConvertFloat4ToU32(labelColor), 
						labelfontDistanceScale);
					
					renderVerticalOffset += (((drawnRect.bottom - drawnRect.top) / 2.f) + (verticalpadding * labelScale));
				}



				// render distance measure
				if (waypoint.showDistance && measurePlayerDistanceToObjectLocked.has_value())
				{
					LOG_ONCE(PLOG_DEBUG << "rendering distance measure only");

					auto& distanceScale = waypoint.distanceScaleUseGlobal ? settings->waypoint3DGlobalDistanceScale->GetValue() : waypoint.distanceScale;
					auto& distanceColor = waypoint.distanceColorUseGlobal ? settings->waypoint3DGlobalDistanceColor->GetValue() : waypoint.distanceColor;

					float distancefontDistanceScale = distanceScale * RenderTextHelper::scaleTextDistance(renderer->cameraDistanceToWorldPoint(waypoint.position));

					// reuse distance from render range test, if we did it
					auto distance = distanceToCamera.has_value() ? distanceToCamera.value() : measurePlayerDistanceToObjectLocked.value()->measure(waypoint.position);
					if (distance.has_value())
					{
						auto distancePrecision = waypoint.distancePrecisionUseGlobal ? settings->waypoint3DGlobalDistancePrecision->GetValue() : waypoint.distancePrecision;

						RenderTextHelper::drawCenteredOutlinedText(
							std::format("{} units", to_string_with_precision(distance.value(), distancePrecision)), 
							{ screenPosition.x, screenPosition.y + renderVerticalOffset }, 
							ImGui::ColorConvertFloat4ToU32(distanceColor),
							distancefontDistanceScale);
					}
					else
					{
						RenderTextHelper::drawCenteredOutlinedText(
							"Measurement Error", 
							{ screenPosition.x, screenPosition.y + renderVerticalOffset }, 
							ImGui::ColorConvertFloat4ToU32(distanceColor),
							distancefontDistanceScale);
					}

				}
				
			
			}
		}
		catch (HCMRuntimeException ex)
		{
			PLOG_ERROR << "rendering error (probably sprites): " << ex.what();
			runtimeExceptions->handleMessage(ex);
		}
	}

public:
	Waypoint3DImpl(GameState game, IDIContainer& dicon) :
		render3DEventProviderWeak(resolveDependentCheat(Render3DEventProvider)),
		mWaypoint3DToggleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->waypoint3DToggle->valueChangedEvent, [this](bool& n) {onToggleChange(n); }),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		mEditWaypointEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editWaypointEvent, [this](Waypoint& wp, WaypointList& wplist) { onEditWaypointEvent(wp, wplist); }),
		mDeleteWaypointEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->deleteWaypointEvent, [this](Waypoint& wp, WaypointList& wplist) {onDeleteWaypointEvent(wp, wplist); }),
		mAddWaypointEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->addWaypointEvent, [this](WaypointList& wplist) {onAddWaypointEvent(wplist); }),
		modalDialogsWeak(dicon.Resolve<ModalDialogRenderer>())
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

		try
		{
			getPlayerDatumOptionalWeak = resolveDependentCheat(GetPlayerDatum);
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "failed to resolve getPlayerDatumOptionalWeak service: " << ex.what();
		}

		try
		{
			getObjectPhysicsOptionalWeak = resolveDependentCheat(GetObjectPhysics);
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "failed to resolve getObjectPhysicsOptionalWeak service: " << ex.what();
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