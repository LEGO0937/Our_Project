#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include "MyInclude.h"
#include "protocol.h"

#define SERVER_IP "127.0.0.1"
#pragma comment(lib, "Ws2_32.lib")
constexpr int MAX_BUFFER = 1024;
//constexpr int MAX_USER = 6;
constexpr int SERVER_PORT = 9000;



constexpr int MAX_WORKER_THREAD = 3;

enum EVENT_TYPE
{
	EV_RECV,
	EV_SEND,
	EV_COUNT,
	EV_GO_LOBBY
};

enum COLLISION_TYPE		//어느 객체와 충돌했는지(혹시 몰라서 넣었다) 
{
	CL_NONE,
	CL_ITEM,	//아이템
	CL_PLAYER		//플레이어
};

enum GAME_STATE			//로비 상태인지 인게임 중인지 
{
	GS_ID_INPUT,
	GS_LOBBY,
	GS_INGAME
};

// Overlapped구조체 확장
struct OVER_EX {
	WSAOVERLAPPED	over;
	WSABUF			dataBuffer;
	char			messageBuffer[MAX_BUFFER];
	EVENT_TYPE		event_t;
};

class SOCKETINFO
{
public:
	// in_use와 x,y에서 data race발생
	// in_use가 true인걸 확인하고 send하려고 하는데 그 이전에 접속 종료해서 false가된다면?
	// 그리고 send 전에 새 플레이어가 id를 재사용한다면? => 엉뚱한 곳에 send가 됨
	// mutex access_lock;
	bool in_use;
	OVER_EX over_ex;
	SOCKET socket;
	// 조립불가한 메모리를 다음번에 조립하기 위한 임시저장소
	char packet_buffer[MAX_BUFFER];
	int prev_size;
	XMFLOAT4X4 xmf4x4Parents[6] = {};
	char rank; //등수
	char item;
	char animation;
	char nickname[32];

	COLLISION_TYPE collision;
	bool isReady;
	GAME_STATE gameState;
public:
	SOCKETINFO() {
		in_use = false;
		rank = 0;
		item = ITEM::EMPTY;
		ZeroMemory(nickname, sizeof(wchar_t) * 12);
		ZeroMemory(&over_ex.messageBuffer, sizeof(over_ex.messageBuffer));
		ZeroMemory(&packet_buffer, sizeof(packet_buffer));
		over_ex.dataBuffer.len = MAX_BUFFER;
		over_ex.dataBuffer.buf = over_ex.messageBuffer;
		over_ex.event_t = EV_RECV;
	}
	void InitPlayer(){
		rank = 0;
		isReady = false;
		item = ITEM::EMPTY;
	}
};

class Server
{
private:
	SOCKET listenSocket;
	HANDLE iocp;
	mutex clientCnt_l;
	mutex readyCnt_l;
	// vector로 했을 때 over_ex.messagebuffer에 값이 들어오질 않는다.
	// 배열로 바꾸니 제대로 동작함. 왜? 무슨 차이?
	SOCKETINFO clients[MAX_USER];
	vector<thread> workerThreads;
	int clientCount;
	int readyCount;
	int hostId;
public:
	Server();
	~Server();
public:
	static void AcceptThread(LPVOID arg);
	void AcceptThreadFunc();
	static void WorkerThread(LPVOID arg);
	void WorkerThreadFunc();
public:
	void ProcessPacket(char client, char* packet);
	void SendFunc(char client, void* packet);
	void RecvFunc(char client);
	void ClientDisconnect(char client);
public:
	void SendAcessComplete(char client);
	void SendGoLobby(char toClient);
	//void SendPutPlayer(char toClient, char fromClient);
	void SendReadyStatePacket(char toClient, char fromClient);
	void SendUnReadyStatePacket(char toClient, char fromClient);
	void SendRemovePlayer(char toClient, char fromClient);
	void SendPlayerInfo(char toClietn, char fromClient);
	void SendGetItem(char toClient, char fromClient, string& itemidx);
	void SendUseItem(char toClient, char fromClient, char useItem);

public:
	void SetAnimationState(char client, char animationNum);
	void SetClient_Initialize(char client);

public:
	bool InitServer();
	void RunServer();
public:
	void err_quit(const char*);
	void err_display(const char*);
};

