#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <map>

#define SERVERPORT 9000
#define BUFSIZE    512

// 체스판 칸
const int ROW = 8;
const int COL = 8;

// 패킷타입
const int TYPE_ENTER = 0;
const int TYPE_INGAME = 1;
const int TYPE_EXIT = 2;

const int MAX_CLIENT = 10;

using namespace std;

struct POSITION {
	POSITION() :x(0), y(0) {}
	POSITION(int x1, int y1) :x(x1), y(y1) {}
	int x;
	int y;
};

struct ClientInfo
{
	POSITION pos;
};

struct CS_SOCK {
	int key;
};

struct SC_SOCK {
	int type;
	int myId;
	ClientInfo clientInfo;
};

struct SC_INIT_SOCK {
	int myId;
	int clientId[MAX_CLIENT];
	POSITION clientInfo[MAX_CLIENT];
};

struct CONNECT_SOCK {
	bool isConnected;
};

struct Clients
{
public:
	WSAOVERLAPPED overlapped;
	WSABUF wsabuf;
	int myId;
	CS_SOCK csSock;
	POSITION pos;
	SOCKET sock;
public:
	Clients() :pos(0, 0) {};
	Clients(POSITION p) : pos(p.x, p.y) {};
public:
	// 복사대입과 복사생성 방지
	//Clients(const Clients&) = delete;
	Clients& operator=(const Clients&) = delete;
};

void CALLBACK RecvCompletionRoutine(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
void CALLBACK SendCompletionRoutine(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);

// 소켓 함수 오류 출력 후 종료
void err_quit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void err_display(const int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMsgBuf, 0, NULL);
	printf("[오류] %s", (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

std::map<SOCKET, Clients> mClient;
CRITICAL_SECTION cs;
POSITION tmpP;
int acceptedClient = 0;

int main(int argc, char *argv[])
{
	InitializeCriticalSection(&cs);
	int clientCount = 0;
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	// 서버의 지역 IP주소와 포트번호 9000번을 설정
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);			// 네트워크에서 이 포트번호를 보고 찾아와야 하기 때문에 네트워크의 사용방식인 빅엔디안으로 저장한다.
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	// listen_sock이라는 소켓과 결합된 포트 상태를 대기상태로 변경
	// 클라이언트의 connect 요청이 있을 때 까지 대기하며
	// SOMAXCONN 수만큼 소켓 생성 가능
	retval = listen(listen_sock, MAX_CLIENT);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	while (true)
	{
		// accept()
		client_sock = accept(listen_sock, NULL, NULL);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		CONNECT_SOCK connectSock;
		acceptedClient++;

		if (acceptedClient > MAX_CLIENT)
		{
			connectSock.isConnected = false;
			printf("접속 가능한 클라이언트 수를 초과했습니다.\n");

			retval = send(client_sock, (char *)&connectSock, sizeof(connectSock), 0);
			if (retval == SOCKET_ERROR)
				err_display("WSASend()");

			closesocket(client_sock);
			acceptedClient--;
			continue;
		}
		else
		{
			connectSock.isConnected = true;
			retval = send(client_sock, (char *)&connectSock, sizeof(connectSock), 0);
			if (retval == SOCKET_ERROR)
				err_display("WSASend()");
		}

		// 접속한 클라이언트 정보 출력
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 소켓 정보 구조체 할당과 초기화
		Clients *ptr = new Clients(tmpP);
		tmpP.x++;
		if (tmpP.x >= COL)
		{
			tmpP.x = 0;
			++tmpP.y;
			if (tmpP.y >= ROW)
				tmpP.y = 0;
		}

		if (ptr == NULL)
		{
			printf("[오류] 메모리가 부족합니다!\n");
			return 1;
		}

		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		mClient.emplace(client_sock, *ptr);
		mClient[client_sock].sock = client_sock;
		mClient[client_sock].myId = clientCount++;
		
		SC_INIT_SOCK scInitSock;
		scInitSock.myId = mClient[client_sock].myId;
		int i = 0;
		for (auto& d : mClient)
		{
			scInitSock.clientInfo[i] = d.second.pos;
			scInitSock.clientId[i++] = d.second.myId;
		}

		//for (auto& d : mClient)
		//	scInitSock.clientInfo.emplace(d.first, POSITION(d.second.pos.x, d.second.pos.y));
		mClient[client_sock].wsabuf.buf = (char *)&scInitSock;
		mClient[client_sock].wsabuf.len = sizeof(SC_INIT_SOCK);

		// 새로 접속한 클라이언트에게 접속 중인 클라이언트들의 정보를 모두 보냄
		retval = WSASend(mClient[client_sock].sock, &mClient[client_sock].wsabuf, 1, NULL, 0, &ptr->overlapped, SendCompletionRoutine);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				err_display("WSASend()");
				return 0;
			}
		}
		//scInitSock.clientInfo.clear();

		SC_SOCK scSock;
		scSock.type = TYPE_ENTER;
		scSock.myId = mClient[client_sock].myId;
		scSock.clientInfo.pos = mClient[client_sock].pos;

		// 새로 접속한 클라이언트의 정보를 모든 클라이언트에 보냄(자기자신빼고)
		for (auto& p : mClient)
		{
			if (p.first != client_sock)
			{
				ZeroMemory(&p.second.overlapped, sizeof(p.second.overlapped));
				p.second.wsabuf.buf = (char *)&scSock;
				p.second.wsabuf.len = sizeof(SC_SOCK);
				retval = WSASend(p.first, &p.second.wsabuf, 1, NULL, 0, &p.second.overlapped, SendCompletionRoutine);
				if (retval == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSA_IO_PENDING)
					{
						err_display("WSASend()");
						return 0;
					}
				}
			}
		}

		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		mClient[client_sock].wsabuf.buf = (char *)&mClient[client_sock].csSock;
		mClient[client_sock].wsabuf.len = sizeof(CS_SOCK);

		// 비동기 입출력 시작
		DWORD flags = 0;
		retval = WSARecv(mClient[client_sock].sock, &mClient[client_sock].wsabuf, 1, NULL, &flags, &mClient[client_sock].overlapped, RecvCompletionRoutine);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				err_display("WSARecv()");
				return 1;
			}
		}
	}

	// closesocket()
	closesocket(listen_sock);
	mClient.clear();
	// 윈속 종료
	WSACleanup();
	return 0;
}

// RECV 비동기 입출력 처리 함수(입출력 완료 루틴)
void CALLBACK RecvCompletionRoutine(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	int retval;

	// 클라이언트 정보 얻기
	// 이 부분이 어떻게 이어지는지 모르겠음...
	Clients *ptr = reinterpret_cast<Clients *>(lpOverlapped);
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);

	// 비동기 입출력 결과 확인
	if (dwError != 0 || cbTransferred == 0)
	{
		if (dwError != 0)
			err_display(dwError);
		SC_SOCK scSock;
		for (auto& d : mClient)
		{
			if (d.first != ptr->sock)
			{
				scSock.type = TYPE_EXIT;
				scSock.myId = ptr->myId;

				ZeroMemory(&d.second.overlapped, sizeof(d.second.overlapped));
				d.second.wsabuf.buf = (char *)&scSock;
				d.second.wsabuf.len = sizeof(SC_SOCK);

				retval = WSASend(d.first, &d.second.wsabuf, 1, NULL, 0, &d.second.overlapped, SendCompletionRoutine);
				if (retval == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSA_IO_PENDING)
					{
						err_display("WSASend()inCR");
						return;
					}
				}
			}
		}
		mClient.erase(ptr->sock);
		closesocket(ptr->sock);
		acceptedClient--;
		printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트 번호 = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		return;
	}

	// recv 후처리 코드
	// 들어온 key와 position을 가지고 연산
	int key = 0;
	POSITION tmpPos;

	key = ptr->csSock.key;
	tmpPos = ptr->pos;

	switch (key)
	{
	case VK_UP:
		if (tmpPos.y > 0)
			tmpPos.y--;
		break;
	case VK_DOWN:
		if (tmpPos.y < ROW - 1)
			tmpPos.y++;
		break;
	case VK_RIGHT:
		if (tmpPos.x < COL - 1)
			tmpPos.x++;
		break;
	case VK_LEFT:
		if (tmpPos.x > 0)
			tmpPos.x--;
		break;
	}
	EnterCriticalSection(&cs);
	ptr->pos = tmpPos;
	LeaveCriticalSection(&cs);

	SC_SOCK scSock;
	scSock.clientInfo.pos = tmpPos;
	scSock.type = TYPE_INGAME;
	scSock.myId = ptr->myId;

	// 해당 플레이어의 연산된 내용을 모든 플레이어에게 send
	for (auto& d : mClient)
	{
		ZeroMemory(&d.second.overlapped, sizeof(d.second.overlapped));
		d.second.wsabuf.buf = (char *)&scSock;
		d.second.wsabuf.len = sizeof(SC_SOCK);

		retval = WSASend(d.first, &d.second.wsabuf, 1, NULL, 0, &d.second.overlapped, SendCompletionRoutine);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				err_display("WSASend()inCR");
				return;
			}
		}
	}

	// 데이터 받기
	// 이거 안 써주면 이어서 안되는듯. 계속해서 recv받기 위해서는 완료루틴을 계속 호출해야 하는 것 같다.
	ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
	// 이렇게 연결해야하나 ptr 자체에 연결할 수 있나
	ptr->wsabuf.buf = (char *)&ptr->csSock;
	ptr->wsabuf.len = sizeof(CS_SOCK);

	DWORD flags = 0;
	retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, NULL, &flags, &ptr->overlapped, RecvCompletionRoutine);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			err_display("WSARecv()inCR");
			return;
		}
	}
}

// SEND 비동기 입출력 처리 함수(입출력 완료 루틴)
void CALLBACK SendCompletionRoutine(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	int retval;

	// 클라이언트 정보 얻기
	Clients *ptr = reinterpret_cast<Clients *>(lpOverlapped);
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);

	// 비동기 입출력 결과 확인
	// 여기서 에러가 남 바인딩 실패. 이미 bind된 소켓에 바인드하거나 주소체계가 일관적이지 않을 때
	// 주소 체계가 일관적이지 않은게 아닐까?
	if (dwError != 0)
	{
		err_display(dwError);
		mClient.erase(ptr->sock);
		closesocket(ptr->sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트 번호 = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		return;
	}

	// send 후처리 코드

}