#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
// 012345 총 6명
const int MAX_CLIENT = 5;

using std::vector;

class Network
{
private:
	// smart pointer를 쓸 것인지 고려 필요.
	SOCKET* mListenSock;
	vector<SOCKET> clientSock;
	short clientNum;
public:
	Network();
	~Network();
public:
	const SOCKET& GetListenSock() const;
	const vector<SOCKET>& GetVectorForSocket() const;
	const short GetClientNum() const;

	void SetListenSock(SOCKET* socket);
	void SetVectorForSocket(SOCKET* socket);
};

