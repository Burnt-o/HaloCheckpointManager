#include "pch.h"
#include "Speedhack.h"
#include "SettingsStateAndEvents.h"
#include "IMessagesGUI.h"
#include "DirPathContainer.h"
#include "RuntimeExceptionHandler.h"
// see HCMSpeedhack.dll


class SpeedhackImpl : public ISpeedhackImpl
{
private:
	// event callbacks
	ScopedCallback<ToggleEvent> mSpeedhackToggleCallbackHandle;
	ScopedCallback<eventpp::CallbackList<void(double&)>> mSpeedhackSettingCallbackHandle;

	// injected services
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<Setting<double>> speedhackValueSettingWeak;
	std::weak_ptr<Setting<bool>> speedhackToggleSettingWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// data
	std::function<void(double)> setSpeed;
public:


	void onToggle(bool& newToggleValue)
	{
		try
		{
			lockOrThrow(speedhackValueSettingWeak, speedhackValueSetting);
			lockOrThrow(messagesGUIWeak, messagesGUI);

			auto currentSpeedSetting = speedhackValueSetting->GetValue();
			PLOG_DEBUG << "SpeedhackImpl recevied onToggle event, value: " << newToggleValue;
			PLOG_DEBUG << "(the speedhack value is: " << currentSpeedSetting << ")";
			if (newToggleValue)
			{
				setSpeed(currentSpeedSetting);
				messagesGUI->addMessage(std::format("Enabling Speedhack ({:.2f}).", currentSpeedSetting));
			}
			else
			{
				setSpeed(1.00);
				messagesGUI->addMessage(std::format("Disabling Speedhack."));
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}


	}
	void updateSetting(double& newSpeedValue)
	{
		try
		{
			lockOrThrow(speedhackToggleSettingWeak, speedhackToggleSetting);
			lockOrThrow(messagesGUIWeak, messagesGUI);

			if (speedhackToggleSetting->GetValue())
			{
				PLOG_DEBUG << "SpeedhackImpl recevied updateSetting event, value: " << newSpeedValue;
				setSpeed(newSpeedValue);
				messagesGUI->addMessage(std::format("Set Speedhack to {:.2f}.", newSpeedValue));
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}

	SpeedhackImpl(GameState game, IDIContainer& dicon)
		: speedhackValueSettingWeak(dicon.Resolve<SettingsStateAndEvents>().lock()->speedhackSetting),
		speedhackToggleSettingWeak(dicon.Resolve<SettingsStateAndEvents>().lock()->speedhackToggle),
		mSpeedhackToggleCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->speedhackToggle->valueChangedEvent, [this](bool& i) {onToggle(i); }),
		mSpeedhackSettingCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->speedhackSetting->valueChangedEvent, [this](double& i) {updateSetting(i); }),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		// load HCMSpeedhack.dll
		auto dirPathCont = dicon.Resolve<DirPathContainer>().lock();
		std::string speedhackPath = dirPathCont->dirPath;
		speedhackPath += "\\HCMSpeedhack.dll";

		if (!fileExists(speedhackPath)) throw HCMInitException(std::format("Speedhack dll missing at {}", speedhackPath));

		auto dllHandle = LoadLibraryA(speedhackPath.c_str());

		if (!dllHandle) throw HCMInitException("Failed to load/find speedhack dll in mcc");

		auto pSetSpeed = GetProcAddress(dllHandle, "setSpeed");

		if (!pSetSpeed) throw HCMInitException("Could not find speedhack function \"setSpeed\"");

		setSpeed = std::function<void(double)>(reinterpret_cast<void(__stdcall*)(double)>(pSetSpeed));


	}
	~SpeedhackImpl()
	{
		setSpeed(1.00);
		PLOG_DEBUG << "~SpeedhackImpl()";
	}
};


Speedhack::Speedhack(GameState game, IDIContainer& dicon) 
{
	// on implemntation covers all games, so only construct it the first time
	if (!impl)
	{
		PLOG_DEBUG << "constructing static speedhack impl";
		impl = std::make_unique<SpeedhackImpl>(game, dicon);
	}

}