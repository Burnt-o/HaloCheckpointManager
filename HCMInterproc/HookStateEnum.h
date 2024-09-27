#pragma once


enum class HookStateEnum
{
    MCCNotFound, // MCC process not found
    MCCAccessError, // Couldn't access MCC process - Admin priv issue
    MCCEACError, // EAC (easy anti-cheat) was running, user needs to launch MCC with eac disabled.
    StateMachineException, // Exceptions in the state machine
    InternalInjecting, // HCMExternal is currently trying to inject HCMInternal
    InternalInjectError, // Something went wrong injecting HCMInternal
    InternalInitialising, // HCMExternal is waiting for HCMInternal to initialise
    InternalException, // HCMInternal threw an error (during initialisation or otherwise)
    InternalSuccess // Everything went fine with HCMInternal injection & initialisation
};