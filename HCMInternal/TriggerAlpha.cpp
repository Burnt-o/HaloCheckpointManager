#include "pch.h"
#include "TriggerAlpha.h"
#include "SettingsStateAndEvents.h"



class TriggerAlphaImpl : public ITriggerAlphaImpl
{
private:

	std::shared_ptr< SettingsStateAndEvents> settings;

	ScopedCallback<eventpp::CallbackList<void(float&)>> triggerOverlayAlphaCallback;

	void onTriggerOverlayAlphaCallback(const float& newValue)
	{
		settings->triggerOverlayNormalColor->GetValueDisplay().w = newValue;
		settings->triggerOverlayNormalColor->UpdateValueWithInput();

		settings->triggerOverlayBSPColor->GetValueDisplay().w = newValue;
		settings->triggerOverlayBSPColor->UpdateValueWithInput();

		
		settings->triggerOverlayCheckHitColor->GetValueDisplay().w = newValue + ((1.f - newValue) / 3.f); // lil extra alpha for hits
		settings->triggerOverlayCheckHitColor->UpdateValueWithInput();

		settings->triggerOverlayCheckMissColor->GetValueDisplay().w = newValue;
		settings->triggerOverlayCheckMissColor->UpdateValueWithInput();
	}

public:
	TriggerAlphaImpl(GameState gameImpl, IDIContainer& dicon)
		: settings(dicon.Resolve< SettingsStateAndEvents>()),
		triggerOverlayAlphaCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->triggerOverlayAlpha->valueChangedEvent, [this](float& n) { onTriggerOverlayAlphaCallback(n); })
	{
		onTriggerOverlayAlphaCallback(settings->triggerOverlayAlpha->GetValue());
	}
};


TriggerAlpha::TriggerAlpha(GameState gameImpl, IDIContainer& dicon)
{
	if (!pimpl)
		pimpl = std::make_unique<TriggerAlphaImpl>(gameImpl, dicon);
}

TriggerAlpha::~TriggerAlpha() = default;
