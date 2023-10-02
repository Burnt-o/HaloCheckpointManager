#include "pch.h"
#include "RPCClientInternal.h"



bool RPCClientInternal::sendHeartbeat()
{
	if (client.get_connection_state() != rpc::client::connection_state::connected)
	{
		PLOG_ERROR << "Internal client not connected!";
		return false;
	}

	try
	{
		return client.call("heartbeat").as<bool>();
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
	if (client.get_connection_state() != rpc::client::connection_state::connected)
		throw HCMRuntimeException("Internal client not connected!");

	try
	{
		return (SelectedCheckpointData)client.call("getInjectInfo").as<SelectedCheckpointDataExternal>();
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
	if (client.get_connection_state() != rpc::client::connection_state::connected)
		throw HCMRuntimeException("Internal client not connected!");

	try
	{
		return (SelectedFolderData)client.call("getDumpInfo").as<SelectedFolderDataExternal>();
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


void RPCClientInternal::sendFatalInternalError(std::string err)
{
	if (client.get_connection_state() != rpc::client::connection_state::connected)
	{
		PLOG_ERROR << "sendFatalInternalError failed: rpc not connected";
			return;
	}

	try
	{
		rpc::client temp_client{"127.0.0.1", 8545};
		temp_client.set_timeout(3000);
		temp_client.async_call("fatalInternalError", err);
	}
	catch (rpc::rpc_error& e)
	{
		PLOG_ERROR << (std::format("sendFatalInternalError failed: {}", e.what()));
	}
	catch (rpc::timeout& e)
	{
		PLOG_ERROR << (std::format("sendFatalInternalError timed out: {}", e.what()));
	}
	catch (rpc::system_error& e)
	{
		PLOG_ERROR << (std::format("sendFatalInternalError system errored: {}", e.what()));
	}

}