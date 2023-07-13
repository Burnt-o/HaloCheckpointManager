#include "pch.h"
#include "RPCServer.h"

#include "Events.h"
#include "CheckpointInjectPath.h"
#include "CommandQueue.h"
#include <filesystem>

bool RPCServer::connectionEstablised = false;

std::string requestHCMDirectory()
{
    RPCServer::connectionEstablised = true;
    //CHAR buffer[MAX_PATH] = { 0 };
    //GetModuleFileNameA(NULL, buffer, MAX_PATH);

    return std::filesystem::current_path().generic_string();
}

std::string requestCheckpointToInject()
{
    return GetCheckpointInjectPath();
}


int heartBeat()
{
    return (int)GetCommand();
}


RPCServer::RPCServer()
{
	PLOG_DEBUG << "RPCServer constructing";
    connectionEstablised = false;

    // Create bindings
    srv.bind("requestHCMDirectory", &requestHCMDirectory);

    srv.bind("fatalInternalError", [](std::string err) {
        ErrorEventInvoke(err.c_str());
        });

    srv.bind("requestCheckpointToInject", &requestCheckpointToInject);

    srv.bind("heartBeat", &heartBeat);
    PLOG_DEBUG << "Bindings done";
    // Run the server loop.
    srv.async_run();
    PLOG_DEBUG << "Server running";

}

RPCServer::~RPCServer()
{
	PLOG_DEBUG << "RPCServer destructing";
    srv.stop();
    connectionEstablised = false;
}