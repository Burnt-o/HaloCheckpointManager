#include "pch.h"
#include "RPCClient.h"

RPCClient* RPCClient::instance = nullptr;


// WHEW okay so this if my first time really diving into async stuff, so uh I might be doing things wrong here



checkpointInjectInfoInternal RPCClient::requestInjectInfo(GameState game)
{
	try 
	{
		PLOG_DEBUG << "RPCClient::requestInjectInfo";
		return instance->client.call("requestInjectInfo", (int)game).as<checkpointInjectInfoInternal>();
	}
    catch (rpc::rpc_error& e)
    {
        throw HCMRuntimeException(std::format("HCMInternal failed to call requestInjectInfo: {}", e.what()));
    }
	catch (rpc::timeout& e)
	{
		throw HCMRuntimeException(std::format("HCMInternal failed to call requestInjectInfo: {}", e.what()));
	}
}

checkpointDumpInfoInternal RPCClient::requestDumpInfo(GameState game) // called by checkpoint dump. External will use this to align tab also.
{
	try
	{
		PLOG_DEBUG << "RPCClient::requestDumpInfo";
		return instance->client.call("requestDumpInfo", (int)game).as<checkpointDumpInfoInternal>();
	}
	catch (rpc::rpc_error& e)
	{
		throw HCMRuntimeException(std::format("HCMInternal failed to call requestDumpInfo: {}", e.what()));
	}
	catch (rpc::timeout& e)
	{
		throw HCMRuntimeException(std::format("HCMInternal failed to call requestDumpInfo: {}", e.what()));
	}
}


int RPCClient::sendHeartbeat()
{
	try
	{
		return instance->client.call("heartbeat").as<int>();
	}
	catch (rpc::rpc_error& e)
	{
		PLOG_ERROR << "heartbeat failed: " << e.what();
		return -1;
	}
	catch (rpc::timeout& e)
	{
		PLOG_ERROR << "heartbeat timed out: " << e.what();
		return -1;
	}
}