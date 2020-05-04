#include "Network.h"



Network::Network()
{
	*mListenSock = NULL;
	clientSock.reserve(3);
	clientNum = 0;
}


Network::~Network()
{
	if(mListenSock)
		delete mListenSock;
	clientSock.clear();
}

const SOCKET& Network::GetListenSock() const
{
	return *mListenSock;
}
const vector<SOCKET>& Network::GetVectorForSocket() const
{
	return clientSock;
}
const short Network::GetClientNum() const 
{
	return clientNum; 
}

void Network::SetListenSock(SOCKET* socket) 
{ 
	if (!mListenSock)
		mListenSock = new SOCKET(*socket);
}
void Network::SetVectorForSocket(SOCKET* socket)
{
	if (clientNum < MAX_CLIENT)
	{
		clientSock.emplace_back(*socket);
		clientNum++;
	}
	else
		printf("더 이상 접속할 수 없습니다.\n");
}