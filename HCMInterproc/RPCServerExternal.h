#pragma once
#include "rpc/server.h"
#include "ExternalInfo.h"


class RPCServerExternal
{
private:
	// Creating a server that listens on port 8069
	rpc::server srv{ 8069 };

public:
	static bool connectionEstablised;

	RPCServerExternal();
	~RPCServerExternal();

	static SelectedCheckpointDataExternal cII;
	static SelectedFolderDataExternal cDI;
};

