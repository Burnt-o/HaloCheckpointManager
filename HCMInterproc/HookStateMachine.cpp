#include "pch.h"
#include "HookStateMachine.h"
#include "WinHandle.h"
#include <optional>
#include "HookStateEnum.h"
#include "WindowsUtilities.h"
#include "Events.h"
#include "FindProcess.h"
#include "ImbueInternal.h"
#include "SharedMemoryExternal.h"

class HookStateMachine::HookStateMachineImpl
{
private:

	HookStateEnum currentState = HookStateEnum::MCCNotFound;
	std::optional<HandlePtr> mccHandle;

	void implAdvanceStateMachine(HookStateEnum newState)
	{
		if (currentState == newState)
		{
			PLOG_ERROR << "Tried advancing state machine to same state: " << magic_enum::enum_name(newState);
			return;
		}
		else
		{
            PLOG_INFO << "Advancing state from " << magic_enum::enum_name(currentState) << " to " << magic_enum::enum_name(newState);
			currentState = newState;
            HookStateChangedEvent_Invoke(currentState);
			updateStateMachine();
		}
	}

    void implAdvanceStateMachineAndError(HookStateEnum newState, std::wstring errorMessage)
    {

            PLOG_INFO << "Advancing state from " << magic_enum::enum_name(currentState) << " to " << magic_enum::enum_name(newState);
            currentState = newState;
            HookStateChangedEvent_Invoke(currentState);
            ErrorEvent_Invoke(errorMessage.c_str());
            updateStateMachine();
    }


	void updateStateMachine()
	{
    #define ADVANCE_STATE_MACHINE(newState) implAdvanceStateMachine(newState); return;
    #define ADVANCE_STATE_MACHINE_ERROR(newState, message) implAdvanceStateMachineAndError(newState, message); return;
		switch (currentState)
		{
		case HookStateEnum::MCCNotFound:
		{


			// Find Valid MCC process(loop / return if unfound)
            LOG_ONCE(PLOG_VERBOSE << "checking for valid mcc process");
            mccHandle = findValidMCCProcess();
            if (!mccHandle) // no process found
            {
                LOG_ONCE(PLOG_VERBOSE << "none found");
                return;
            }

            PLOG_INFO << "Found a valid MCC process!";

			// Check for EAC (error if found)
			if (anticheatIsEnabled())
			{
                ADVANCE_STATE_MACHINE_ERROR(HookStateEnum::MCCEACError, L"HCM detected EasyAntiCheat running - you must run MCC with easy anti-cheat disabled.");
			}
			else
			{
				// Advance to Injection state
				ADVANCE_STATE_MACHINE(HookStateEnum::InternalInjecting);
			}
		}
		break;

        case HookStateEnum::MCCAccessError:
			// Do nothing
			break;

        case HookStateEnum::MCCEACError:
			// Do nothing
			break;

        case HookStateEnum::StateMachineException:
			// Do nothing
			break;

        case HookStateEnum::InternalInjecting:
        {
            if (!mccHandle)
            {
                ADVANCE_STATE_MACHINE(HookStateEnum::MCCNotFound);
            }

            auto imbueErrorString = imbueInternal(mccHandle.value());

            if (imbueErrorString)
            {
                ADVANCE_STATE_MACHINE_ERROR(HookStateEnum::InternalInjectError, str_to_wstr(std::format(
                    "HCM failed to inject its internal module into the game! \nMore info in HCMExternal.log file. Error message: \n{}",
                    wstr_to_str(imbueErrorString.value())
                )).c_str());
            }
            else
            {
                ADVANCE_STATE_MACHINE(HookStateEnum::InternalInitialising);
            }
        }
            break;

        case HookStateEnum::InternalInitialising:
        {
            HCMInternalStatus currentInternalState = getHCMInternalStatusFlag();
            switch (currentInternalState)
            {
            case HCMInternalStatus::Initialising:
                break; // do nothing (keep waiting)

            case HCMInternalStatus::Error:
                ADVANCE_STATE_MACHINE_ERROR(HookStateEnum::InternalException, L"HCMInternal had an exception during intialisation - see MCC window (or HCMInternal.log) for more details");
                break;


            case HCMInternalStatus::AllGood:
                ADVANCE_STATE_MACHINE(HookStateEnum::InternalSuccess);
                break;

            }
        }
            break;


        case HookStateEnum::InternalInjectError:
            // Do nothing
            break;

        case HookStateEnum::InternalException:
            // Do nothing
            break;

        case HookStateEnum::InternalSuccess:
        {
            // Check status flag to make sure no errors have popped up, otherwise do nothing (send heartbeat
            HCMInternalStatus currentInternalState = getHCMInternalStatusFlag();
            PLOG_VERBOSE << std::format("InternalState: {}" , currentInternalState);

            // check for mcc shutdown
            if (currentInternalState == HCMInternalStatus::Shutdown || hasMCCTerminated(mccHandle.value()))
            {
                ADVANCE_STATE_MACHINE(HookStateEnum::MCCNotFound);
            }

            if (currentInternalState == HCMInternalStatus::Error)
            {
                ADVANCE_STATE_MACHINE_ERROR(HookStateEnum::InternalException, L"HCMInternal had an exception - see MCC window (or HCMInternal.log) for more details");
            }


        }
            break;

		}
	}



public:
	void update()
	{
		updateStateMachine();
	}

    void reset()
    {
        ADVANCE_STATE_MACHINE(HookStateEnum::MCCNotFound);
    }

};


HookStateMachine::HookStateMachine() : pimpl(std::make_unique< HookStateMachineImpl>()) {}

HookStateMachine::~HookStateMachine() = default;

void HookStateMachine::update() {
   pimpl->update(); 
}

void HookStateMachine::reset()
{
    pimpl->reset();
}