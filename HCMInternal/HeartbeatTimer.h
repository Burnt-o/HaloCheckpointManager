#pragma once
#include "WinHandle.h"
class HeartbeatTimer
{
public:
    explicit HeartbeatTimer();

    ~HeartbeatTimer();



private:
    std::thread _thd;
    HandlePtr HCMExternalHandle;
};

