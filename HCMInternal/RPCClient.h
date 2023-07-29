#pragma once
#include <rpc\client.h>
#include <rpc\rpc_error.h>
#include "Logging.h"
#include "HCMDirPath.h"
#include "HaloEnums.h"
#include "InjectRequirements.h"
#include "CheckpointInfo.h"

class RPCClient
{
private:
    static RPCClient* instance;
    rpc::client client{"127.0.0.1", 8069};

public:
    RPCClient()
    {
        if (instance != nullptr) throw HCMInitException("Cannot have more than one RPCClient");
        instance = this;


        client.set_timeout(1000);
        std::string result = "none";
        try
        {
            result = client.call("requestHCMDirectory").as<std::string>();
            HCMDirPath::SetHCMDirPath(result);
            Logging::initLogging();
            Logging::SetConsoleLoggingLevel(plog::verbose);
            Logging::SetFileLoggingLevel(plog::verbose);

        }
        catch (rpc::rpc_error& e)
        {
            Logging::initLogging();
            Logging::SetConsoleLoggingLevel(plog::verbose);
            throw HCMInitException(std::format("HCMInternal failed to establish communication with HCMExternal: {}", e.what()));
        }


    }

    static checkpointInjectInfoInternal requestInjectInfo(GameState game);
    static checkpointDumpInfoInternal requestDumpInfo(GameState game); // called by checkpoint dump. External will use this to align tab also.
    static int sendHeartbeat();
};

