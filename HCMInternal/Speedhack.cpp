#include "pch.h"
#include "Speedhack.h"
#include "HCMDirPath.h"

// see HCMSpeedhack.dll


class SpeedhackImpl : public SpeedhackImplBase 
{
private:

	eventpp::CallbackList<void(bool&)>::Handle mSpeedhackToggleCallbackHandle = {};
	eventpp::CallbackList<void(double&)>::Handle mSpeedhackSettingCallbackHandle = {};

	std::function<void(double)> setSpeed;
public:


	virtual void onToggle(bool& newValue) override
	{
		PLOG_DEBUG << "SpeedhackImpl recevied onToggle event, value: " << newValue;
		PLOG_DEBUG << "(the speedhack value is: " << OptionsState::speedhackSetting.get()->GetValue() << ")";
		if (newValue)
		{
			setSpeed(OptionsState::speedhackSetting.get()->GetValue());
		}
		else
		{
			setSpeed(1.00);
		}
	}
	virtual void updateSetting(double& newValue) override
	{
		PLOG_DEBUG << "SpeedhackImpl recevied updateSetting event, value: " << newValue;
		setSpeed(newValue);
	}

	SpeedhackImpl()
	{
		// subscribe to gui events
		mSpeedhackToggleCallbackHandle = OptionsState::speedhackToggle.get()->valueChangedEvent.append([this](bool& i) {onToggle(i); });
		mSpeedhackSettingCallbackHandle = OptionsState::speedhackSetting.get()->valueChangedEvent.append([this](double& i) {updateSetting(i); });

		// load HCMSpeedhack.dll
		std::string speedhackPath = HCMDirPath::GetHCMDirPath().data();
		speedhackPath += "\\HCMSpeedhack.dll";

		if (!fileExists(speedhackPath)) throw HCMInitException(std::format("Speedhack dll missing at {}", speedhackPath));

		auto dllHandle = LoadLibraryA(speedhackPath.c_str());

		if (!dllHandle) throw HCMInitException("Failed to load/find speedhack dll in mcc");

		auto pSetSpeed = GetProcAddress(dllHandle, "setSpeed");

		if (!pSetSpeed) throw HCMInitException("Could not find speedhack function \"setSpeed\"");

		setSpeed = std::function<void(double)>(reinterpret_cast<void (__stdcall *)(double)>(pSetSpeed));

	}
	~SpeedhackImpl()
	{
		// unsubscribe to gui events
		if (mSpeedhackToggleCallbackHandle)
			OptionsState::speedhackToggle.get()->valueChangedEvent.remove(mSpeedhackToggleCallbackHandle);

		if (mSpeedhackSettingCallbackHandle)
			OptionsState::speedhackSetting.get()->valueChangedEvent.remove(mSpeedhackSettingCallbackHandle);

		setSpeed(1.00);
	}
};

// we only need one implementation that covers all games
void Speedhack::initialize()
{
	if
		(impl.get()) return;
	else
		impl = std::make_unique<SpeedhackImpl>();

}