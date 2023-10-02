#include "pch.h"
#include "RPCServerExternal.h"

#include "Events.h"
#include <filesystem>
#include "WindowsUtilities.h"

bool RPCServerExternal::connectionEstablised = false;
SelectedCheckpointDataExternal RPCServerExternal::cII{};
SelectedFolderDataExternal RPCServerExternal::cDI{};
std::string requestHCMDirectory()
{
    RPCServerExternal::connectionEstablised = true;

    return std::string(std::filesystem::current_path().generic_string() + "/");
}

SelectedCheckpointDataExternal getInjectInfo()
{
    return RPCServerExternal::cII;
}

SelectedFolderDataExternal getDumpInfo()
{
    return RPCServerExternal::cDI;
}

bool heartbeat()
{
    return true;
}


RPCServerExternal::RPCServerExternal()
{
	PLOG_DEBUG << "RPCServer constructing";
    connectionEstablised = false;

    // Create bindings
    srv.bind("requestHCMDirectory", &requestHCMDirectory);

    srv.bind("fatalInternalError", [](std::string err) {
        ErrorEventInvoke(err.c_str());
        });

    srv.bind("heartbeat", &heartbeat);

    srv.bind("getInjectInfo", &getInjectInfo);
    srv.bind("getDumpInfo", &getDumpInfo);

    PLOG_DEBUG << "Bindings done";
    // Run the server loop.
    srv.async_run();
    PLOG_DEBUG << "Server running";

}

RPCServerExternal::~RPCServerExternal()
{
	PLOG_DEBUG << "RPCServer destructing";
    srv.stop();
    connectionEstablised = false;
}