#include "pch.h"
#include "SoftCeilingOverlay.h"
#include "GetSoftCeilingData.h"
#include "Render3DEventProvider.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "IMessagesGUI.h"


template <GameState::Value gameT>
class SoftCeilingOverlayImpl : public SoftCeilingOverlayUntemplated
{
private:
	// callbacks
	std::unique_ptr<ScopedCallback<Render3DEvent>> mRenderEventCallback;
	ScopedCallback<ToggleEvent> softCeilingOverlayToggleEventCallback;


	// injected services
	std::weak_ptr<Render3DEventProvider> render3DEventProviderWeak;
	std::weak_ptr<GetSoftCeilingData> getSoftCeilingDataWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMessagesGUI> messagesWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// data
	std::atomic_bool renderingMutex = false;


	void onToggleChange(bool& newValue)
	{
		lockOrThrow(mccStateHookWeak, mccStateHook);
		if (mccStateHook->isGameCurrentlyPlaying(gameT) == false)
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
				PLOG_DEBUG << "subscribing to render event!";
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

		try
		{
			lockOrThrow(messagesWeak, messages);
			messages->addMessage(newValue ? "Barrier Overlay enabled." : "Barrier Overlay disabled.");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}



	

	// new frame, render
	void onRenderEvent(GameState game, IRenderer3D* renderer)
	{
		if (game.operator GameState::Value() != gameT) return;

		try // renderer funcs can throw HCMRuntime exceptions
		{
			ScopedAtomicBool lockRender(renderingMutex);



			LOG_ONCE(PLOG_DEBUG << "acquiring soft ceiling data");

			lockOrThrow(getSoftCeilingDataWeak, getSoftCeilingData);
			auto softCeilingDataLock = getSoftCeilingData->getSoftCeilings();

			if (!softCeilingDataLock)
				throw softCeilingDataLock.error(); // todo; figure out a neater way to handle errors

			LOG_ONCE(PLOG_DEBUG << "successfully acquired soft ceiling data");

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "rendering triangle count: " << std::hex << c, c = softCeilingDataLock.value().get()->size());

			lockOrThrow(settingsWeak, settings);
			auto desiredSubjects = settings->softCeilingOverlayRenderTypes->GetValue();
			bool wantsVehicles = desiredSubjects == SettingsEnums::SoftCeilingRenderTypes::Vehicles || desiredSubjects == SettingsEnums::SoftCeilingRenderTypes::BipedsOrVehicles;
			bool wantsBipeds = desiredSubjects == SettingsEnums::SoftCeilingRenderTypes::Bipeds || desiredSubjects == SettingsEnums::SoftCeilingRenderTypes::BipedsOrVehicles;

			auto renderDirection = settings->softCeilingOverlayRenderDirection->GetValue();

			auto cullingOption = renderDirection == SettingsEnums::SoftCeilingRenderDirection::Both ? CullingOption::CullNone :
				renderDirection == SettingsEnums::SoftCeilingRenderDirection::Front ? CullingOption::CullBack : CullingOption::CullFront;

			for (auto& softCeiling : *softCeilingDataLock.value().get())
			{
				bool isDesiredSubject = (wantsVehicles && softCeiling.appliesToVehicle()) || (wantsBipeds && softCeiling.appliesToBiped());
				if (!isDesiredSubject)
					continue; 

				renderer->drawTriangleCollection(&softCeiling, softCeiling.colorSolid, cullingOption);
				renderer->drawEdgeCollection(&softCeiling, softCeiling.colorWireframe);

			}
			




		}
		catch (HCMRuntimeException ex)
		{
			PLOG_ERROR << "Soft Ceiling Overlay Rendering error: " << std::endl << ex.what();
			runtimeExceptions->handleMessage(ex);

			try
			{
				lockOrThrow(settingsWeak, settings);
				settings->softCeilingOverlayToggle->GetValueDisplay() = false;
				settings->softCeilingOverlayToggle->UpdateValueWithInput();
			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
		}
	}

public:
	SoftCeilingOverlayImpl(GameState game, IDIContainer& dicon) :
		render3DEventProviderWeak(resolveDependentCheat(Render3DEventProvider)),
		getSoftCeilingDataWeak(resolveDependentCheat(GetSoftCeilingData)),
		softCeilingOverlayToggleEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->softCeilingOverlayToggle->valueChangedEvent, [this](bool& n) {onToggleChange(n); }),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesWeak(dicon.Resolve<IMessagesGUI>())

	{

	}

	~SoftCeilingOverlayImpl()
	{
		if (renderingMutex)
		{
			renderingMutex.wait(true);
		}
		mRenderEventCallback.reset();
	}


};


SoftCeilingOverlay::SoftCeilingOverlay(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		throw HCMInitException("SoftCeilingOverlay not applicable to h1");
		break;

	case GameState::Value::Halo2:
		throw HCMInitException("SoftCeilingOverlay not applicable to h2");
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<SoftCeilingOverlayImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<SoftCeilingOverlayImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<SoftCeilingOverlayImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<SoftCeilingOverlayImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException(std::format("{} not impl yet", nameof(SoftCeilingOverlay)));
	}
}

SoftCeilingOverlay::~SoftCeilingOverlay() = default;