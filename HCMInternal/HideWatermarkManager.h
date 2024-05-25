#pragma once
#include "Lapua.h"
#include "RuntimeExceptionHandler.h"
#include "BinarySetting.h"


class HideWatermarkManager
{
private:

	// injected services
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<HCMInternalGUI> hcmInternalGUIWeak;

	ScopedCallback<ToggleEvent> HideWatermarkToggleEventCallback;
	std::shared_ptr<BinarySetting<bool>> HideWatermarkToggle;

	void onHideWatermarkToggleEvent(bool& newValue)
	{
		PLOGV << "onHideWatermarkToggleEvent firing, newValue: " << newValue;
		try
		{

			if (newValue)
			{
				// todo fix this up
				if (Lapua::lapuaGood == false) throw HCMRuntimeException("Lapua service one failure");
				if (Lapua::lapuaGood2 == false) throw HCMRuntimeException("Lapua service two failure");
				Lapua::shouldRunForWatermark = true;
			}
			else
			{
				Lapua::shouldRunForWatermark = false;
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			HideWatermarkToggle->GetValueDisplay() = false;
			HideWatermarkToggle->UpdateValueWithInput();
		}

	}


public:
	HideWatermarkManager(std::shared_ptr<BinarySetting<bool>> toggle, std::shared_ptr<RuntimeExceptionHandler> exp)
		:
		HideWatermarkToggle(toggle),
		runtimeExceptions(exp),
		HideWatermarkToggleEventCallback(toggle->valueChangedEvent, [this](bool& n) { onHideWatermarkToggleEvent(n); })
	{}

};