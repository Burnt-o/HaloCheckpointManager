#pragma once
#include "WinHandle.h"
#include "SharedMemoryInternal.h"
#include "SettingsStateAndEvents.h"
class HeartbeatTimer
{
private:
    std::weak_ptr<SharedMemoryInternal> sharedMemWeak;
    std::weak_ptr<SettingsStateAndEvents> settingsWeak;

public:
    explicit HeartbeatTimer(std::weak_ptr<SharedMemoryInternal>, std::weak_ptr<SettingsStateAndEvents>);

    ~HeartbeatTimer();



private:
    std::thread _thd;
    HandlePtr HCMExternalHandle;
};

