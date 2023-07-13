#pragma once
#include "rpc/server.h"



class RPCServer
{
private:
	// Creating a server that listens on port 8069
	rpc::server srv{ 8069 };

public:
	static bool connectionEstablised;

	RPCServer();
	~RPCServer();

};

