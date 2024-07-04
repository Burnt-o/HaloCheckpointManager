#include "pch.h"
#include "SwitchBSPSet.h"
#include "GetCurrentBSPSet.h"
#include "IMakeOrGetCheat.h"
#include "MultilevelPointer.h"
#include "RuntimeExceptionHandler.h"
#include "IMCCStateHook.h"
#include "PointerDataStore.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include <bitset>
class SwitchBSPSet::SwitchBSPSetImpl
{
private:


	// data
	GameState mGame;
	std::shared_ptr<MultilevelPointer> currentBSPSet;

	// callbacks
	ScopedCallback<ActionEvent> switchBSPSetLoadSetEventCallback;
	ScopedCallback<ActionEvent> switchBSPLoadIndexEventCallback;
	ScopedCallback<ActionEvent> switchBSPUnloadIndexEventCallback;
	ScopedCallback<ActionEvent> switchBSPSetFillCurrentCallback;


	// injected services
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<GetCurrentBSPSet> getCurrentBSPSetWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	void onSwitchBSPSetLoadSetEvent()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			BSPSet bspSet = BSPSet(settings->switchBSPSetLoadSet->GetValue());
			LoadBSPSet(bspSet);
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onSwitchBSPLoadIndexEvent()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			auto bspIndex = settings->switchBSPSetLoadIndex->GetValue();

			LoadBSPByIndex(bspIndex);
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onSwitchBSPUnloadIndexEvent()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			auto bspIndex = settings->switchBSPSetUnloadIndex->GetValue();

			UnloadBSPByIndex(bspIndex);
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onSwitchBSPSetFillCurrent()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			lockOrThrow(getCurrentBSPSetWeak, getCurrentBSPSet);
			auto currentBSPSet = getCurrentBSPSet->getCurrentBSPSet();
			settings->switchBSPSetLoadSet->GetValueDisplay() = currentBSPSet.operator BSPIndexMask().value;
			settings->switchBSPSetLoadSet->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}





public:
	SwitchBSPSetImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		getCurrentBSPSetWeak(resolveDependentCheat(GetCurrentBSPSet)),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		switchBSPSetLoadSetEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->switchBSPSetLoadSetEvent, [this]() {onSwitchBSPSetLoadSetEvent(); }),
		switchBSPLoadIndexEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->switchBSPLoadIndexEvent, [this]() {onSwitchBSPLoadIndexEvent(); }),
		switchBSPUnloadIndexEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->switchBSPUnloadIndexEvent, [this]() {onSwitchBSPUnloadIndexEvent(); }),
		switchBSPSetFillCurrentCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->switchBSPSetFillCurrent, [this]() {onSwitchBSPSetFillCurrent(); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		currentBSPSet = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(currentBSPSet), game);
	}

	void LoadBSPSet(BSPSet bspSet)
	{
		// check if highest bit of bspSet exceeds maxIndex
		lockOrThrow(getCurrentBSPSetWeak, getCurrentBSPSet);
		auto maxIndex = getCurrentBSPSet->getMaxBSPIndex();

		if (bspSet.highestLoadedIndex() > maxIndex)
			throw HCMRuntimeException(std::format("Highest set bit of BSP set {} exceeded max index of {}", bspSet.highestLoadedIndex(), maxIndex));

		PLOG_DEBUG << "Setting bsp set to value: 0x" << bspSet.toHexadecimalString();

		if (!currentBSPSet->writeData(&bspSet))
			throw HCMRuntimeException(std::format("Failed to write BSP set, {}", MultilevelPointer::GetLastError()));

		lockOrThrow(messagesGUIWeak, messages);
		messages->addMessage(std::format("Loaded BSP Set: {}", bspSet.toBinaryString(getCurrentBSPSet->getMaxBSPIndex())));
		
	}
	void UnloadBSPByIndex(int BSPindex)
	{
		lockOrThrow(getCurrentBSPSetWeak, getCurrentBSPSet);
		auto maxIndex = getCurrentBSPSet->getMaxBSPIndex();
		
		if (BSPindex > maxIndex) 
			throw HCMRuntimeException(std::format("BSPIndex {} exceeded max index of {}", BSPindex, maxIndex));

		auto bspSet = getCurrentBSPSet->getCurrentBSPSet();

		// check if bsp was already unloaded
		if (bspSet.indexIsLoaded(BSPindex) == false)
			throw HCMRuntimeException(std::format("BSPIndex {} was already unloaded!", BSPindex));

		// set corrosponding bit to false in currentSet
		bspSet.setIndex(BSPindex, false);

		PLOG_DEBUG << "Setting bsp set to value: 0x" << bspSet.toHexadecimalString();

		if (!currentBSPSet->writeData(&bspSet))
			throw HCMRuntimeException(std::format("Failed to write BSP set, {}", MultilevelPointer::GetLastError()));

		lockOrThrow(messagesGUIWeak, messages);
		messages->addMessage(std::format("Loaded BSP Set: {}", bspSet.toBinaryString(getCurrentBSPSet->getMaxBSPIndex())));
	}
	void LoadBSPByIndex(int BSPindex)
	{
		lockOrThrow(getCurrentBSPSetWeak, getCurrentBSPSet);
		auto maxIndex = getCurrentBSPSet->getMaxBSPIndex();

		if (BSPindex > maxIndex)
			throw HCMRuntimeException(std::format("BSPIndex {} exceeded max index of {}", BSPindex, maxIndex));

		auto bspSet = getCurrentBSPSet->getCurrentBSPSet();

		// check if bsp was already loaded
		if (bspSet.indexIsLoaded(BSPindex) == true)
			throw HCMRuntimeException(std::format("BSPIndex {} was already loaded!", BSPindex));

		// set corrosponding bit to true in currentSet
		bspSet.setIndex(BSPindex, true);

		PLOG_DEBUG << "Setting bsp set to value: 0x" << bspSet.toHexadecimalString();

		if (!currentBSPSet->writeData(&bspSet))
			throw HCMRuntimeException(std::format("Failed to write BSP set, {}", MultilevelPointer::GetLastError()));

		lockOrThrow(messagesGUIWeak, messages);
		messages->addMessage(std::format("Loaded BSP Set: {}", bspSet.toBinaryString(getCurrentBSPSet->getMaxBSPIndex())));
	}
};



SwitchBSPSet::SwitchBSPSet(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< SwitchBSPSetImpl>(gameImpl, dicon);
}
SwitchBSPSet::~SwitchBSPSet() = default;

void SwitchBSPSet::LoadBSPSet(BSPSet bspSet) { return pimpl->LoadBSPSet(bspSet); }
void SwitchBSPSet::UnloadBSPByIndex(int BSPindex) { return pimpl->UnloadBSPByIndex(BSPindex); }
void SwitchBSPSet::LoadBSPByIndex(int BSPindex) { return pimpl->LoadBSPByIndex(BSPindex); }