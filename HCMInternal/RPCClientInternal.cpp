#include "pch.h"
#include "RPCClientInternal.h"

RPCClientInternal* RPCClientInternal::instance = nullptr;



bool RPCClientInternal::sendHeartbeat()
{
	if (instance->client.get_connection_state() != rpc::client::connection_state::connected)
	{
		PLOG_ERROR << "Internal client not connected!";
		return false;
	}

	try
	{
		return instance->client.call("heartbeat").as<bool>();
	}
	catch (rpc::rpc_error& e)
	{
		PLOG_ERROR << "heartbeat failed: " << e.what();
		return false;
	}
	catch (rpc::timeout& e)
	{
		PLOG_ERROR << "heartbeat timed out: " << e.what();
		return false;
	}
	catch (rpc::system_error& e)
	{
		PLOG_ERROR << "heartbeat system errored: " << e.what();
		return false;
	}
}

SelectedCheckpointData RPCClientInternal::getInjectInfo()
{
	if (instance->client.get_connection_state() != rpc::client::connection_state::connected)
		throw HCMRuntimeException("Internal client not connected!");

	try
	{
		return (SelectedCheckpointData)instance->client.call("getInjectInfo").as<SelectedCheckpointDataExternal>();
	}
	catch (rpc::rpc_error& e)
	{
		throw HCMRuntimeException(std::format("getInjectInfo failed: {}", e.what()));
	}
	catch (rpc::timeout& e)
	{
		throw HCMRuntimeException(std::format("getInjectInfo timed out: {}", e.what()));
	}
	catch (rpc::system_error& e)
	{
		throw HCMRuntimeException(std::format("getInjectInfo system errored: {}", e.what()));
	}
}

SelectedFolderData RPCClientInternal::getDumpInfo()
{
	if (instance->client.get_connection_state() != rpc::client::connection_state::connected)
		throw HCMRuntimeException("Internal client not connected!");

	try
	{
		return (SelectedFolderData)instance->client.call("getDumpInfo").as<SelectedFolderDataExternal>();
	}
	catch (rpc::rpc_error& e)
	{
		throw HCMRuntimeException(std::format("getDumpInfo failed: {}", e.what()));
	}
	catch (rpc::timeout& e)
	{
		throw HCMRuntimeException(std::format("getDumpInfo timed out: {}", e.what()));
	}
	catch (rpc::system_error& e)
	{
		throw HCMRuntimeException(std::format("getDumpInfo system errored: {}", e.what()));
	}
}
