#include "pch.h"
#include "ViewAngle3D.h"
#include "PointerDataStore.h"
#include "ViewAngleLineList.h"
#include "Render3DEventProvider.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "MeasurePlayerDistanceToObject.h"
#include "RenderTextHelper.h"
#include "ModalDialogRenderer.h"
#include "GetPlayerViewAngle.h"
#include "ModalDialogFactory.h"

// TODO: scale font/sprite size by screen size (actually should I?)

//https://stackoverflow.com/a/43482688
struct separate_thousands : std::numpunct<char> {
	char_type do_thousands_sep() const override { return ','; }  // separate with commas
	string_type do_grouping() const override { return "\3"; } // groups of 3 digit
};

static std::locale thousandsSeperatedLocale2 = std::locale(std::cout.getloc(), new separate_thousands);


template<GameState::Value mGame>
class ViewAngle3DImpl : public ViewAngle3DUntemplated {

private:
	// callbacks
	std::unique_ptr<ScopedCallback<Render3DEvent>> mRenderEventCallback;
	std::atomic_bool renderingMutex = false;
	ScopedCallback<ToggleEvent> mViewAngle3DToggleEventCallback;
	ScopedCallback<ViewAngleLineAndListEvent> mEditViewAngleEventCallback;
	ScopedCallback<ViewAngleLineAndListEvent> mDeleteViewAngleEventCallback;
	ScopedCallback<ViewAngleLineListEvent> mAddViewAngleEventCallback;


	bool gameValid = false;

	// injectedservices
	std::weak_ptr<Render3DEventProvider> render3DEventProviderWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<ModalDialogRenderer> modalDialogsWeak;
	std::weak_ptr<GetPlayerViewAngle> getPlayerViewAngleWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// for the "fill with player angle" button on the viewAngleLine add/edit dialog
	std::optional<SubpixelID> getPlayerAngleForDialog()
	{

		try
		{
			lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
			return SubpixelID::fromFloat(getPlayerViewAngle->getPlayerViewAngle().x);
		}
		catch (HCMRuntimeException ex)
		{
			ex.prepend("Couldn't parse player view angle for viewAngleLine dialog: ");
			runtimeExceptions->handleMessage(ex);
			return std::nullopt;
		}
	}

	void onEditViewAngleEvent(ViewAngleLine& val, ViewAngleLineList& vallist)
	{
		// call modal dialog, passing original viewAngleLine. Dialog will make a copy of this viewAngleLine which the user will edit.
		// Dialog will return a viewAngleLine if user proceeds, or std::nullopt if they cancel.
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;


			lockOrThrow(modalDialogsWeak, modalDialogs);
			auto maybeNewViewAngle = modalDialogs->showReturningDialog(ModalDialogFactory::makeEditOrAddViewAngleLineDialog(runtimeExceptions, "Edit ViewAngleLine", val, getPlayerAngleForDialog()));

			if (maybeNewViewAngle.has_value())
			{
				if (vallist.listInUse)
				{
					vallist.listInUse.wait(true);
				}
				ScopedAtomicBool lock(vallist.listInUse);
				val = maybeNewViewAngle.value(); // replace original wp by ref
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onDeleteViewAngleEvent(ViewAngleLine& val, ViewAngleLineList& vallist)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

			if (vallist.listInUse)
			{
				vallist.listInUse.wait(true);
			}
			ScopedAtomicBool lock(vallist.listInUse);

			// delete the viewAngleLine from the list if the pointer matches
			vallist.list.erase(
				std::remove_if(vallist.list.begin(), vallist.list.end(),
					[&val](const ViewAngleLine& valToCheck) { return &valToCheck == &val; }),
				vallist.list.end());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}

	void onAddViewAngleEvent(ViewAngleLineList& vallist)
	{
		// call modal dialog, passing a default view angle line. 
		// Dialog will return a viewAngleLine if user proceeds, or std::nullopt if they cancel.
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

			auto defaultAngle = SubpixelID::fromFloat(0.000000000001f);
			try
			{
					lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
					defaultAngle = SubpixelID::fromFloat(getPlayerViewAngle->getPlayerViewAngle().x);
				PLOG_ERROR << "error getting player position for new view angle line, continuing with 0.0f";
			}
			catch (HCMRuntimeException ex)
			{
				ex.prepend("error getting player position for new view angle line, continuing with 0.0f");
				PLOG_ERROR << ex.what();
			}


			ViewAngleLine defaultViewAngle = ViewAngleLine(defaultAngle);

			lockOrThrow(modalDialogsWeak, modalDialogs);
			auto maybeNewViewAngle = modalDialogs->showReturningDialog(ModalDialogFactory::makeEditOrAddViewAngleLineDialog(runtimeExceptions, "Add View Angle Line", defaultViewAngle, getPlayerAngleForDialog()));

			if (maybeNewViewAngle.has_value())
			{
				if (vallist.listInUse)
				{
					vallist.listInUse.wait(true);
				}
				ScopedAtomicBool lock(vallist.listInUse);
				vallist.list.emplace_back(maybeNewViewAngle.value()); // add new view angle line to end of list
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}


	// when user toggles whether 3d view angle lines are enabled
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
				mRenderEventCallback = render3DEventProvider->getRender3DEvent()->subscribe([this](GameState g, IRenderer3D* n) {onRenderEvent(g, n); });

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
	void onRenderEvent(GameState game, IRenderer3D* renderer)
	{
		if (game.operator GameState::Value() != mGame) return;

		try // renderer funcs can throw HCMRuntime exceptions
		{
			ScopedAtomicBool lockRender(renderingMutex);
			LOG_ONCE(PLOG_VERBOSE << "onRenderEvent");


			lockOrThrow(settingsWeak, settings);

			if (settings->viewAngleLine3DList->GetValue().listInUse)
			{
				LOG_ONCE(PLOG_DEBUG << "ViewAngle3D::onRenderEvent waiting for view angle list to no longer be locked");
				settings->viewAngleLine3DList->GetValue().listInUse.wait(true);
			}

			ScopedAtomicBool lock(settings->viewAngleLine3DList->GetValue().listInUse);
			LOG_ONCE(PLOG_DEBUG << "ViewAngle3D::onRenderEvent locking view angle line list");

			auto& lockedViewAngleLineList = settings->viewAngleLine3DList->GetValue().list;

#ifdef HCM_DEBUG
			if (GetKeyState('7') & 0x8000)
			{
				PLOG_DEBUG << "lockedViewAngleLineList.size(): " << lockedViewAngleLineList.size();
			}
#endif

			if (!lockedViewAngleLineList.empty())
			{
				const auto cameraOffsetInfinitelyDown = SimpleMath::Vector3(0, 0, -1000);
				const auto cameraOffsetInfinitelyUp = SimpleMath::Vector3(0, 0, 1000);

				const auto currentCameraPosition = renderer->getCameraPosition();

				lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);

				const auto currentPlayerViewAngle = getPlayerViewAngle->getPlayerViewAngle().x;

				for (auto& viewAngleLine : lockedViewAngleLineList)
				{
					if (viewAngleLine.viewAngleLineEnabled == false) continue;

					// convert subpixel ID to angle
					float angle = viewAngleLine.subpixelID;

					// setup some vars we'll reuse
					float renderVerticalOffset = 0.f;
					constexpr float verticalpadding = 10.f;
					constexpr float horizontalpadding = 10.f;

					// convert anle to a vector relative to the player (magnitude of this vector doesn't really matter)
					const auto cameraOffsetAngle = SimpleMath::Vector3(std::cosf(angle), std::sinf(angle), 0);

					// add to camera and get screen pos
					auto midLineWorldPosition = currentCameraPosition + (cameraOffsetAngle * 100.f);
					auto midLineScreenPosition = renderer->worldPointToScreenPosition(midLineWorldPosition);


					float rightDistance = angle - currentPlayerViewAngle;
					float leftDistance = (angle + 6.283185482f) - currentPlayerViewAngle;


					bool midPointbehindCamera = ((std::fabs(rightDistance) > 1.57) && (std::fabs(leftDistance) > 1.57));

#ifdef HCM_DEBUG
					if (GetKeyState('7') & 0x8000)
					{
						PLOG_DEBUG << "rightDistance: " << rightDistance;
						PLOG_DEBUG << "leftDistance: " << leftDistance;
						PLOG_DEBUG << "abs rightDistance: " << std::fabs(rightDistance);
						PLOG_DEBUG << "abs leftDistance: " << std::fabs(leftDistance);
						PLOG_DEBUG << "midPointbehindCamera " << (midPointbehindCamera ? "true" : "false");
					}
#endif

					// render line
					{
						auto spriteColor = viewAngleLine.spriteColorUseGlobal ? settings->viewAngleLine3DGlobalSpriteColor->GetValue() : viewAngleLine.spriteColor;
						renderer->drawEdge(
							midLineWorldPosition + cameraOffsetInfinitelyDown,
							midLineWorldPosition + cameraOffsetInfinitelyUp,
							spriteColor);
					}

					if (!midPointbehindCamera)
					{

						auto distancePrecision = viewAngleLine.distancePrecisionUseGlobal ? settings->viewAngleLine3DGlobalDistancePrecision->GetValue() : viewAngleLine.distancePrecision;
						//  render label
						if (viewAngleLine.showLabel)
						{
							LOG_ONCE(PLOG_DEBUG << "rendering viewAngleLine label");
							auto labelScale = viewAngleLine.labelScaleUseGlobal ? settings->viewAngleLine3DGlobalLabelScale->GetValue() : viewAngleLine.labelScale;
							auto labelColor = viewAngleLine.labelColorUseGlobal ? settings->viewAngleLine3DGlobalLabelColor->GetValue() : viewAngleLine.labelColor;

							if (viewAngleLine.label.empty() == false)
							{
								// draw main label text
								auto drawnRect = RenderTextHelper::drawOutlinedText(
									viewAngleLine.label,
									{ midLineScreenPosition.x + horizontalpadding, midLineScreenPosition.y + renderVerticalOffset },
									ImGui::ColorConvertFloat4ToU32(labelColor),
									labelScale);

								renderVerticalOffset += ((drawnRect.bottom - drawnRect.top) * 3 + (verticalpadding * labelScale / 16.f));
							}

							//  render angle
							if (viewAngleLine.showAngle)
							{
								LOG_ONCE(PLOG_DEBUG << "rendering viewAngleLine angle");

								auto drawnRect = RenderTextHelper::drawOutlinedText(
									std::format("Angle: {} radians", to_string_with_precision(angle, distancePrecision)),
									{ midLineScreenPosition.x + horizontalpadding, midLineScreenPosition.y + renderVerticalOffset },
									ImGui::ColorConvertFloat4ToU32(labelColor),
									labelScale);

								renderVerticalOffset += ((drawnRect.bottom - drawnRect.top) * 3 + (verticalpadding * labelScale / 16.f));
							}

							//  render subpixel
							if (viewAngleLine.showSubpixelID)
							{
								LOG_ONCE(PLOG_DEBUG << "rendering viewAngleLine subpixel");

								std::stringstream ss;
								ss << viewAngleLine.subpixelID;

								auto drawnRect = RenderTextHelper::drawOutlinedText(
									std::format("Subpixel ID: {}", ss.str()),
									{ midLineScreenPosition.x + horizontalpadding, midLineScreenPosition.y + renderVerticalOffset },
									ImGui::ColorConvertFloat4ToU32(labelColor),
									labelScale);

								renderVerticalOffset += ((drawnRect.bottom - drawnRect.top) * 3 + (verticalpadding * labelScale / 16.f));
							}

						}

						

						// render distance measure
						if (viewAngleLine.showDistance)
						{
							LOG_ONCE(PLOG_DEBUG << "rendering distance measure");

							auto distanceScale = viewAngleLine.distanceScaleUseGlobal ? settings->viewAngleLine3DGlobalDistanceScale->GetValue() : viewAngleLine.distanceScale;
							auto distanceColor = viewAngleLine.distanceColorUseGlobal ? settings->viewAngleLine3DGlobalDistanceColor->GetValue() : viewAngleLine.distanceColor;
							
							{
								auto drawnRect = RenderTextHelper::drawOutlinedText(
									"Distance: ",
									{ midLineScreenPosition.x + horizontalpadding, midLineScreenPosition.y + renderVerticalOffset },
									ImGui::ColorConvertFloat4ToU32(distanceColor),
									distanceScale);

								renderVerticalOffset += ((drawnRect.bottom - drawnRect.top) * 3 + (verticalpadding * distanceScale / 16.f));
							}

							auto measureAsSubpixels = viewAngleLine.measureAsSubpixels;

							if (measureAsSubpixels)
							{
								constexpr int maxSubpixelInt = 1078530012;


								const int subpixelDistanceRight =   ((uint32_t)viewAngleLine.subpixelID - (uint32_t)SubpixelID::fromFloat(currentPlayerViewAngle));
								const int subpixelDistanceLeft = maxSubpixelInt - (uint32_t)SubpixelID::fromFloat(currentPlayerViewAngle);

								// we decide off the ANGLE distance which to choose
								const auto subpixelDistance = std::abs(rightDistance) < std::abs(leftDistance) ?
									subpixelDistanceRight :
									subpixelDistanceLeft;
#ifdef HCM_DEBUG
								if (GetKeyState('7') & 0x8000)
								{
									PLOG_DEBUG << "subpixelDistanceLeft " << subpixelDistanceLeft;
									PLOG_DEBUG << "subpixelDistanceRight " << subpixelDistanceRight;
									PLOG_DEBUG << "(uint32_t)SubpixelID::fromFloat(currentPlayerViewAngle) " << (uint32_t)SubpixelID::fromFloat(currentPlayerViewAngle);
									PLOG_DEBUG << "(uint32_t)viewAngleLine.subpixelID " << (uint32_t)viewAngleLine.subpixelID;
								}

#endif

								std::stringstream ss;
								ss.imbue(thousandsSeperatedLocale2);
								ss << subpixelDistance;
								ss << " Subpixels";


								RenderTextHelper::drawOutlinedText(
									ss.str(),
									{ midLineScreenPosition.x + horizontalpadding, midLineScreenPosition.y + renderVerticalOffset },
									ImGui::ColorConvertFloat4ToU32(distanceColor),
									distanceScale);
							}
							else
							{

								const float angleDistance = std::abs(rightDistance) < std::abs(leftDistance) ?
									rightDistance :
									leftDistance;


								RenderTextHelper::drawOutlinedText(
									std::format("{} radians", to_string_with_precision(angleDistance, distancePrecision)),
									{ midLineScreenPosition.x + horizontalpadding, midLineScreenPosition.y + renderVerticalOffset },
									ImGui::ColorConvertFloat4ToU32(distanceColor),
									distanceScale);
							}

						}
					}
				}

			}



		}
		catch (HCMRuntimeException ex)
		{
			PLOG_ERROR << "rendering error: " << ex.what();
			runtimeExceptions->handleMessage(ex);
		}
	}

public:
	ViewAngle3DImpl(GameState game, IDIContainer& dicon) :
		render3DEventProviderWeak(resolveDependentCheat(Render3DEventProvider)),
		mViewAngle3DToggleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->viewAngleLine3DToggle->valueChangedEvent, [this](bool& n) {onToggleChange(n); }),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		mEditViewAngleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editViewAngleLineEvent, [this](ViewAngleLine& wp, ViewAngleLineList& vallist) { onEditViewAngleEvent(wp, vallist); }),
		mDeleteViewAngleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->deleteViewAngleLineEvent, [this](ViewAngleLine& wp, ViewAngleLineList& vallist) {onDeleteViewAngleEvent(wp, vallist); }),
		mAddViewAngleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->addViewAngleLineEvent, [this](ViewAngleLineList& vallist) {onAddViewAngleEvent(vallist); }),
		modalDialogsWeak(dicon.Resolve<ModalDialogRenderer>()),
		getPlayerViewAngleWeak(resolveDependentCheat(GetPlayerViewAngle))

	{
		PLOG_DEBUG << "init ViewAngle3DImpl";
		gameValid = mccStateHookWeak.lock()->isGameCurrentlyPlaying(mGame);
	}
};


ViewAngle3D::ViewAngle3D(GameState gameImpl, IDIContainer& dicon)
{
	PLOG_DEBUG << "about to init ViewAngle3DImpl";

	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<ViewAngle3DImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<ViewAngle3DImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<ViewAngle3DImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<ViewAngle3DImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<ViewAngle3DImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<ViewAngle3DImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

ViewAngle3D::~ViewAngle3D()
{
	PLOG_VERBOSE << "~" << getName();
}