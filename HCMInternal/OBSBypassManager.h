#pragma once
#include "D3D11Hook.h"
#include "Lapua.h"
#include "RuntimeExceptionHandler.h"
#include "Setting.h"


class OBSBypassManager
{
private:

	// injected services
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<D3D11Hook> d3d11HookWeak;

	ScopedCallback<ToggleEvent> OBSBypassToggleEventCallback;
	std::shared_ptr<Setting<bool>> OBSBypassToggle;

	void onOBSBypassToggleEvent(bool& newValue)
	{
		PLOGV << "onOBSBypassToggleEvent firing, newValue: " << newValue;
		try
		{
			lockOrThrow(d3d11HookWeak, d3d11Hook);

			if (newValue)
			{
				// todo fix this up
				if (Lapua::lapuaGood == false) throw HCMRuntimeException("Lapua service one failure");
				if (Lapua::lapuaGood2 == false) throw HCMRuntimeException("Lapua service two failure");
				d3d11Hook->setOBSBypass(true);
			}
			else
			{
				d3d11Hook->setOBSBypass(false);
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			OBSBypassToggle->GetValueDisplay() = false;
			OBSBypassToggle->UpdateValueWithInput();
		}

	}


public:
	OBSBypassManager(std::weak_ptr<D3D11Hook> d3d11Hook, std::shared_ptr<Setting<bool>> toggle, std::shared_ptr<RuntimeExceptionHandler> exp)
		:
		d3d11HookWeak(d3d11Hook),
		OBSBypassToggle(toggle),
		runtimeExceptions(exp),
		OBSBypassToggleEventCallback(toggle->valueChangedEvent, [this](bool& n) { onOBSBypassToggleEvent(n); })
	{}

};