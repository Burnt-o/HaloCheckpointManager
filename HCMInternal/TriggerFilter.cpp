#include "pch.h"
#include "TriggerFilter.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "GetTriggerData.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "boost\algorithm\string\trim.hpp"


class TriggerFilterImpl
{

private:
	GameState mGame;

	// injected services
	std::weak_ptr<GetTriggerData> getTriggerDataWeak;
	std::weak_ptr< IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::shared_ptr< SettingsStateAndEvents> settings;


	// callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;
	ScopedCallback<eventpp::CallbackList<void(std::string& newValue)>> filterStringChangedCallback;
	ScopedCallback<ToggleEvent> filterToggleChangedCallback;
	ScopedCallback<ToggleEvent> filterExactMatchChangedCallback;
	ScopedCallback<ToggleEvent> triggerOverlayToggleCallback;
	ScopedCallback<eventpp::CallbackList<void(void)>> triggerDataChangedCallback;

	void onToggleChanged()
	{
		onFilterStringChanged(settings->triggerOverlayFilterString->GetValue());
	}

	void onFilterStringChanged(const std::string& newValue)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false)
				return;


			lockOrThrow(getTriggerDataWeak, getTriggerData);


			if (settings->triggerOverlayToggle->GetValue() && settings->triggerOverlayFilterToggle->GetValue())
			{
				

				PLOG_DEBUG << "filtering trigger data by string: " << std::endl << newValue;
				std::stringstream stringstream(newValue);

				// string.. is the name, bool is whether a match has been found yet (init to false)
				std::vector<std::pair<std::string, bool>> delimitedTriggerFilter;
				std::string tmp;
				while (std::getline(stringstream, tmp, ';')) // delimit by semicolon
				{
					boost::algorithm::trim(tmp); // remove leading and trailing whitespace
					tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end()); // remove newlines

#ifdef HCM_DEBUG
					PLOG_DEBUG << tmp;
#endif


					if (tmp.size() > 0)
						delimitedTriggerFilter.push_back(std::pair<std::string, bool>(tmp, false));
				}

				auto filteredTriggerData = getTriggerData->getFilteredTriggers();
				auto allTriggerData = getTriggerData->getAllTriggers();
				filteredTriggerData->clear();

				lockOrThrow(messagesWeak, messages);
				if (delimitedTriggerFilter.size() == 0)
				{
					messages->addMessage("Trigger Filter: Empty Trigger Filter List! Don't forget to add trigger names to the filter");
					return;
				}

				std::function<bool(std::string, std::string)> filterMatchesTriggerName = settings->triggerOverlayFilterExactMatch->GetValue() ?
					[](std::string triggerName, std::string filterEntry) { return triggerName == filterEntry; } :
					[](std::string triggerName, std::string filterEntry) { return triggerName.contains(filterEntry); };


				for (auto& filterName : delimitedTriggerFilter)
				{
					for (auto& [triggerPointer, triggerData] : *allTriggerData.get())
					{
						
						if (filterMatchesTriggerName(triggerData.name, filterName.first))
						{
							filterName.second = true;
							filteredTriggerData->emplace(triggerPointer, triggerData);
							// you'd think we ought to break here, but a filter name can match multiple triggers! ie triggers can share a name
						}

					}
				}


				int matchedFilterCount = std::ranges::count_if(delimitedTriggerFilter, [](const auto& f) { return f.second; });


				if (delimitedTriggerFilter.size() != matchedFilterCount)
				{
					std::stringstream unmatchedss;
					for (auto& filter : delimitedTriggerFilter)
					{
						if (!filter.second)
							unmatchedss << filter.first << std::endl;
					}
					messages->addMessage(std::format("The following filter entries didn't find a match: \n{}", unmatchedss.str()));

					messages->addMessage(std::format("Trigger Filter: Only {}/{} filter entries found matching trigger names. Did you make a typo?", matchedFilterCount, delimitedTriggerFilter.size()));
					
				}
				else
				{
					messages->addMessage(std::format("Trigger Filter: Matched {} filter entries.", matchedFilterCount));
				}


			}
			else if (settings->triggerOverlayToggle->GetValue())
			{
				PLOG_DEBUG << "unfiltering trigger data";
				auto filteredTriggerData = getTriggerData->getFilteredTriggers();
				auto allTriggerData = getTriggerData->getAllTriggers();
				filteredTriggerData->clear();

				for (auto& [triggerPointer, triggerData] : *allTriggerData.get())
				{
					filteredTriggerData->emplace(triggerPointer, triggerData);
				}
			}
			


		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

public:
	TriggerFilterImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		filterStringChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterString->valueChangedEvent, [this](const std::string& n) {onFilterStringChanged(n); }),
		filterToggleChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterToggle->valueChangedEvent, [this](bool& n) {onToggleChanged(); }),
		filterExactMatchChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayFilterExactMatch->valueChangedEvent, [this](bool& n) {onToggleChanged(); }),
		triggerOverlayToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayToggle->valueChangedEvent, [this](bool& n) {onToggleChanged(); }),
		getTriggerDataWeak(resolveDependentCheat(GetTriggerData)),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		mccStateHookWeak(dicon.Resolve< IMCCStateHook>()),
		settings(dicon.Resolve<SettingsStateAndEvents>()),
		messagesWeak(dicon.Resolve<IMessagesGUI>()),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState&) {onToggleChanged(); }),
		triggerDataChangedCallback(resolveDependentCheat(GetTriggerData)->getTriggerDataChangedEvent(), [this]() {onToggleChanged(); })
	{

	}

};

TriggerFilter::TriggerFilter(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<TriggerFilterImpl>(gameImpl, dicon);
}

TriggerFilter::~TriggerFilter() = default;