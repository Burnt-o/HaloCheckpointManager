#pragma once
#include <rpc\client.h>
#include <rpc\rpc_error.h>
#include "ExternalInfo.h"
class RPCClientInternal
{
private:
    rpc::client& client;

public:
    RPCClientInternal(rpc::client& inclient) : client(inclient)
    {
        client.set_timeout(1000);
    }
    void sendFatalInternalError(std::string err);

    bool sendHeartbeat();

    SelectedCheckpointData getInjectInfo();
    SelectedFolderData getDumpInfo();

};

