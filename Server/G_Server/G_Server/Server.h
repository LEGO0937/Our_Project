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

enum COLLISION_TYPE		//��� ��ü�� �浹�ߴ���(Ȥ�� ���� �־���) 
{
	CL_NONE,
	CL_ITEM,	//������
	CL_PLAYER		//�÷��̾�
};

enum GAME_STATE			//�κ� �������� �ΰ��� ������ 
{
	GS_ID_INPUT,
	GS_LOBBY,
	GS_INGAME
};

// Overlapped����ü Ȯ��
struct OVER_EX {
	WSAOVERLAPPED	over;
	WSABUF			dataBuffer;
	char			messageBuffer[MAX_BUFFER];
	EVENT_TYPE		event_t;
};

class SOCKETINFO
{
public:
	// in_use�� x,y���� data race�߻�
	// in_use�� true�ΰ� Ȯ���ϰ� send�Ϸ��� �ϴµ� �� ������ ���� �����ؼ� false���ȴٸ�?
	// �׸��� send ���� �� �÷��̾ id�� �����Ѵٸ�? => ������ ���� send�� ��
	// mutex access_lock;
	bool in_use;
	OVER_EX over_ex;
	SOCKET socket;
	// �����Ұ��� �޸𸮸� �������� �����ϱ� ���� �ӽ������
	char packet_buffer[MAX_BUFFER];
	int prev_size;
	XMFLOAT4X4 xmf4x4Parents[6] = {};
	char rank; //���
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
	// vector�� ���� �� over_ex.messagebuffer�� ���� ������ �ʴ´�.
	// �迭�� �ٲٴ� ����� ������. ��? ���� ����?
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

