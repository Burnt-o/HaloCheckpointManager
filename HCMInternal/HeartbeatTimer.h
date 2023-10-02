#pragma once
#include "RPCClientInternal.h"
#include "HCMExternalCommand.h"
#include "GlobalKill.h"
#include "SettingsStateAndEvents.h"
class HeartbeatTimer
{
public:
    explicit HeartbeatTimer(std::shared_ptr<RPCClientInternal> rpc)
        :rpcref(rpc)
    {
        _thd = std::thread([this]()
            {
                while (!GlobalKill::isKillSet())
                {
                    //PLOG_VERBOSE << "sending heartbeat";
                    auto heartIsBeating = rpcref->sendHeartbeat();
                    
                    if (!heartIsBeating)
                    {
                        // https://youtu.be/APc8QCGOdUE
                        GlobalKill::killMe();
                    }
                   
                    auto nextWakeup = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
                    std::this_thread::sleep_until(nextWakeup);
                }
            });
    }

    ~HeartbeatTimer() {
        PLOG_VERBOSE << "~HeartbeatTimer";
        _thd.join();
        PLOG_VERBOSE << "aye the thread died as it should";
    }



private:
    std::thread _thd;
    std::shared_ptr<RPCClientInternal> rpcref;
};

