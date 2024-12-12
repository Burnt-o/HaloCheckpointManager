#include "pch.h"
#include "EditPlayerViewAngleID.h"
#include "RuntimeExceptionHandler.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "IMakeOrGetCheat.h"
#include "GetPlayerViewAngle.h"
#include "IMCCStateHook.h"

//https://stackoverflow.com/a/43482688
struct separate_thousands : std::numpunct<char> {
	char_type do_thousands_sep() const override { return ','; }  // separate with commas
	string_type do_grouping() const override { return "\3"; } // groups of 3 digit
};

class EditPlayerViewAngleID::EditPlayerViewAngleIDImpl
{
private:
	std::locale thousandsSeperatedLocale = std::locale(std::cout.getloc(), new separate_thousands);
	GameState mImplGame;

	//event callbacks
	ScopedCallback<ActionEvent> editPlayerViewAngleIDSetCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleIDAdjustNegativeCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleIDAdjustPositiveCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleIDFillCurrentCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleIDCopyCallback;
	ScopedCallback<ActionEvent> editPlayerViewAngleIDPasteCallback;


	// injected services
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<GetPlayerViewAngle> getPlayerViewAngleWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	


	void onEditPlayerViewAngleIDSet()
	{
		try
		{

			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;



			lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
			lockOrThrow(settingsWeak, settings);

			getPlayerViewAngle->setPlayerViewAngle(SimpleMath::Vector2(SubpixelIDToViewAngle(settings->editPlayerViewAngleIDInt->GetValue()), getPlayerViewAngle->getPlayerViewAngle().y));


			lockOrThrow(messagesGUIWeak, messagesGUI);
			messagesGUI->addMessage(std::format(thousandsSeperatedLocale, "Setting Subpixel ID to: {}", settings->editPlayerViewAngleIDInt->GetValue()));

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAngleIDAdjustNegative()
	{
		try
		{

			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;


			lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
			lockOrThrow(settingsWeak, settings);
			auto currentSubpixel = ViewAngleToSubpixelID(getPlayerViewAngle->getPlayerViewAngle().x);
			currentSubpixel -= settings->editPlayerViewAngleIDAdjustFactor->GetValue();

			// let setting perform validation
			settings->editPlayerViewAngleIDInt->GetValueDisplay() = currentSubpixel;
			settings->editPlayerViewAngleIDInt->UpdateValueWithInput();

			getPlayerViewAngle->setPlayerViewAngle(SimpleMath::Vector2(SubpixelIDToViewAngle(settings->editPlayerViewAngleIDInt->GetValue()), getPlayerViewAngle->getPlayerViewAngle().y));
			
			{
				lockOrThrow(messagesGUIWeak, messagesGUI);
				messagesGUI->addMessage(std::format(thousandsSeperatedLocale, "Setting Subpixel ID to: {}", settings->editPlayerViewAngleIDInt->GetValue()));
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAngleIDAdjustPositive()
	{
		try
		{

			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;


			lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
			lockOrThrow(settingsWeak, settings);
			auto currentSubpixel = ViewAngleToSubpixelID(getPlayerViewAngle->getPlayerViewAngle().x);
			currentSubpixel += settings->editPlayerViewAngleIDAdjustFactor->GetValue();

			// let setting perform validation
			settings->editPlayerViewAngleIDInt->GetValueDisplay() = currentSubpixel;
			settings->editPlayerViewAngleIDInt->UpdateValueWithInput();

			getPlayerViewAngle->setPlayerViewAngle(SimpleMath::Vector2(SubpixelIDToViewAngle(settings->editPlayerViewAngleIDInt->GetValue()), getPlayerViewAngle->getPlayerViewAngle().y));

			{
				lockOrThrow(messagesGUIWeak, messagesGUI);
				messagesGUI->addMessage(std::format(thousandsSeperatedLocale, "Setting Subpixel ID to: {}", settings->editPlayerViewAngleIDInt->GetValue()));
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAngleIDFillCurrent()
	{
		try
		{

			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;


			lockOrThrow(getPlayerViewAngleWeak, getPlayerViewAngle);
			lockOrThrow(settingsWeak, settings);
			auto playerViewAngle = getPlayerViewAngle->getPlayerViewAngle();
			auto subpixelID = ViewAngleToSubpixelID(playerViewAngle.x);
			PLOG_DEBUG << "filling viewAngle with " << subpixelID;
			settings->editPlayerViewAngleIDInt->GetValueDisplay() = subpixelID;
			settings->editPlayerViewAngleIDInt->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAngleIDCopy()
	{
		try
		{

			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;


			lockOrThrow(settingsWeak, settings);
			settings->editPlayerViewAngleIDInt->serialiseToClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onEditPlayerViewAngleIDPaste()
	{
		try
		{

			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;


			lockOrThrow(settingsWeak, settings);
			settings->editPlayerViewAngleIDInt->deserialiseFromClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}


public:
	EditPlayerViewAngleIDImpl(GameState game, IDIContainer& dicon)
		:mImplGame(game),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		editPlayerViewAngleIDSetCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleIDSet, [this]() {onEditPlayerViewAngleIDSet(); }),
		editPlayerViewAngleIDAdjustNegativeCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleIDAdjustNegative, [this]() {onEditPlayerViewAngleIDAdjustNegative(); }),
		editPlayerViewAngleIDAdjustPositiveCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleIDAdjustPositive, [this]() {onEditPlayerViewAngleIDAdjustPositive(); }),
		editPlayerViewAngleIDFillCurrentCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleIDFillCurrent, [this]() {onEditPlayerViewAngleIDFillCurrent(); }),
		editPlayerViewAngleIDCopyCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleIDCopy, [this]() {onEditPlayerViewAngleIDCopy(); }),
		editPlayerViewAngleIDPasteCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->editPlayerViewAngleIDPaste, [this]() {onEditPlayerViewAngleIDPaste(); }),
		getPlayerViewAngleWeak(resolveDependentCheat(GetPlayerViewAngle))
	{

	}
};




EditPlayerViewAngleID::EditPlayerViewAngleID(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique< EditPlayerViewAngleID::EditPlayerViewAngleIDImpl>(game, dicon);
}

EditPlayerViewAngleID::~EditPlayerViewAngleID()
{
	PLOG_DEBUG << "~" << getName();
}