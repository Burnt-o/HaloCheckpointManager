#include "pch.h"
#include "RPCServer.h"

#include "Events.h"
#include <filesystem>
#include "WindowsUtilities.h"
bool RPCServer::connectionEstablised = false;

std::string requestHCMDirectory()
{
    RPCServer::connectionEstablised = true;
    //CHAR buffer[MAX_PATH] = { 0 };
    //GetModuleFileNameA(NULL, buffer, MAX_PATH);

    return std::filesystem::current_path().generic_string();
}

checkpointInjectInfoInternal requestInjectInfo(int game)
{
    // need to fire event to external, convert and return result.
    PLOG_DEBUG << "RPCServer::requestInjectInfo";
    auto e = InternalRequestsInjectInfoInvoke(game);
    PLOG_DEBUG << "received external injectInfo";


    checkpointInjectInfoInternal working;
    if (e.requestFailed)
    {
        PLOG_ERROR << "request failed!";
        return working;
    }

    working.requestFailed = e.requestFailed;
    working.difficulty = e.difficulty;
    strcpy_s(working.checkpointFilePath, e.checkpointFilePath);
    strcpy_s(working.levelCode, e.levelCode);
    strcpy_s(working.version, e.version);

    return working;
    // do we need to free the external mem?
}


checkpointDumpInfoInternal requestDumpInfo(int game)
{
    // need to fire event to external, convert and return result.
    PLOG_DEBUG << "RPCServer::requestDumpInfo";
    auto e = InternalRequestsDumpInfoInvoke(game);
    PLOG_DEBUG << "received external dumpInfo";


    checkpointDumpInfoInternal working;
    if (e.requestFailed)
    {
        PLOG_ERROR << "request failed!";
        return working;
    }

    working.requestFailed = e.requestFailed;
    strcpy_s(working.dumpFolderPath, e.dumpFolderPath);
    PLOG_VERBOSE << "constructed working dumpinfointernal";
    return working;
    // do we need to free the external mem?
}


int64_t heartbeat()
{
    PLOG_VERBOSE << "interproc recieved heartbeat message, invoking event to external";
    return HeartbeatEventInvoke();
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

    srv.bind("requestInjectInfo", &requestInjectInfo);

    srv.bind("requestDumpInfo", &requestDumpInfo);

    srv.bind("heartbeat", &heartbeat);
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