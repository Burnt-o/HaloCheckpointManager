#pragma once
#include "RPCClient.h"
#include "HCMExternalCommand.h"
#include "GlobalKill.h"
#include "OptionsState.h"
class HeartbeatTimer
{
public:
    explicit HeartbeatTimer(std::shared_ptr<RPCClient> rpc)
        :rpcref(rpc)
    {
        _thd = std::thread([this]()
            {
                while (!GlobalKill::isKillSet())
                {
                    PLOG_VERBOSE << "sending heartbeat";
                    auto command = rpcref.get()->sendHeartbeat();
                    PLOG_VERBOSE << "heartbeat: " << command;
                    switch (command)
                    {
                    default:
                    case -1: // failed
                        PLOG_INFO << "Heartbeat command failed, brb gonna kill myself";
                        GlobalKill::killMe();
                        break;

                    case (int)HCMExternalCommand::None:
                        // do nothing
                        break;
                    case (int)HCMExternalCommand::Inject:
                        // fire event
                        OptionsState::injectCheckpointEvent();
                        break;
                    case (int)HCMExternalCommand::Dump:
                        // fire event
                        OptionsState::dumpCheckpointEvent();
                        break;
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
    std::shared_ptr<RPCClient> rpcref;
};

