#include "MyInclude.h"
#include "Server.h"

int main(int argc, char* argv[])
{
	Server server;
	if (server.InitServer())
		server.RunServer();
}


