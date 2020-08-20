#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define Default_LEN 10
//#include "MyInclude.h"
#include "protocol.h"
//#include "string"

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

enum DB_Info
{
	DB_Success,
	DB_NoData,
	DB_NoConnect,
	DB_Overlap
};

// Overlapped����ü Ȯ��
struct OVER_EX 
{
	WSAOVERLAPPED	over;
	WSABUF			dataBuffer;
	char			messageBuffer[MAX_BUFFER];
	EVENT_TYPE		event_t;
};


// Ŭ���̾�Ʈ
class SOCKETINFO
{
public:
	// in_use�� x,y���� data race�߻�
	// in_use�� true�ΰ� Ȯ���ϰ� send�Ϸ��� �ϴµ� �� ������ ���� �����ؼ� false���ȴٸ�?
	// �׸��� send ���� �� �÷��̾ id�� �����Ѵٸ�? => ������ ���� send�� ��
	// mutex access_lock;
	bool isInGameReady = false; // �ΰ��ӿ��� ���ҽ� ���尡 �Ϸ����� ����
	bool in_use; // ������ ���� �ȉ��
	OVER_EX over_ex;
	SOCKET socket;
	// �����Ұ��� �޸𸮸� �������� �����ϱ� ���� �ӽ������
	char packet_buffer[MAX_BUFFER];
	int prev_size;
	string id;

	
	// �ΰ��ӿ�
	char rank; //���
	char game_id[10]; // �ΰ��� ���̵�
	// char nickname[32]; // �̰� ���� �޾ƾ��ϳ�...? ���߿� ���� 
	XMFLOAT4X4 xmf4x4Parents = {}; // �÷��̾� ��ġ��
	MessageStruct msg; // �ִϸ��̼�
	int checkPoints; // �ΰ��� �� üũ����Ʈ
	bool isReady; // ���� or �𷹵�
	GAME_STATE gameState; // ���� �� Ŭ���� ���� ����
	DWORD keyState; // �̰͵� �ִϸ��̼� �κ�
	XMFLOAT3 xmf3PutPos = {}; // �ʱ���ġ �����̶�µ�...
	
	
	// ���߿� ���ͷ� �ٲٵ��� ����


	
public:
	SOCKETINFO() {
		in_use = false;
		rank = 0;
		//item = ITEM::EMPTY;
		checkPoints = 0;
		keyState = 0;
		ZeroMemory(&xmf4x4Parents, sizeof(xmf4x4Parents));
		ZeroMemory(&msg, sizeof(msg));
		//ZeroMemory(nickname, sizeof(wchar_t) * 12);
		ZeroMemory(&over_ex.messageBuffer, sizeof(over_ex.messageBuffer));
		ZeroMemory(&packet_buffer, sizeof(packet_buffer));
		ZeroMemory(&xmf3PutPos, sizeof(xmf3PutPos));
		over_ex.dataBuffer.len = MAX_BUFFER;
		over_ex.dataBuffer.buf = over_ex.messageBuffer;
		over_ex.event_t = EV_RECV;
		isReady = false;

	}

	/*
	clients[client].isConnect = true;
	clients[client].prev_size = 0;
	ZeroMemory(&clients[client].over_ex, sizeof(clients[client].over_ex));
	clients[client].over_ex.event_t = EV_RECV;
	clients[client].over_ex.dataBuffer.buf = reinterpret_cast<CHAR*>(clients[client].over_ex.messageBuffer);
	clients[client].over_ex.dataBuffer.len = sizeof(clients[client].over_ex.messageBuffer);
	clients[client].checkPoints = 0;
	clients[client].isReady = false;
	clients[client].
	*/
	
};


// �� ��������(�� ��ȣ) �� �� �ο�, ������, �������� or ���ǵ���


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
	bool game_mode = false; // default = false

	

	// DB�� �Բ� ���
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLWCHAR sz_id[Default_LEN];
	int  db_connect;
	SQLLEN cb_id = 0, cb_connect = 0;

	char  buf[255];
	wchar_t sql_data[255];
	SQLWCHAR sqldata[255] = { 0 };
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
	void SendAccessPlayer(char toClient, char fromClient);
	void SendPutPlayer(char toClient);
	void SendGoLobby(char toClient);
	void SendGameMode(char toClient);
	void SendReadyStatePacket(char toClient, char fromClient);
	void SendUnReadyStatePacket(char toClient, char fromClient);
	void SendRemovePlayer(char toClient, char fromClient);
	void SendPlayerInfo(char toCliet, char fromClient);
	void SendPlayerAni_Collision(char toCliet, char fromClient);
	void SendPlayerAni_Sliding(char toCliet, char fromClient);
	void SendRoundEnd(char client);
	void SendEventPacket(char toClient, const MessageStruct& msg); // �ִϸ��̼� ���� ��Ŷ
	void SendRoundStart(char client);
	void SendRoomInfo(char client);
	void SendResetRoomInfo(char client);

	void SendInGameReady(char client);
	void SendInGameFinish(char client, string name);
public:
	void SetClient_Initialize(char client);
public:
	bool InitServer();
	void RunServer();
public:
	void err_quit(const char*);
	void err_display(const char*);
// for DB
public:
	void init_DB();
	int get_DB_Info(int ci);
	void set_DB_Info(int ci);
	void set_DB_Shutdown(int ci);
	void new_DB_Id(int ci);
};


//struct MessageStruct
//{
//	char msgName;				//��ɾ�: ������Ʈ����, ���� or��ƼŬ �߰� or ��Ȱ��ȭ
//	char shaderName;             //��� ���̴��� ��Ÿ���� ��������� global.h�� �� ����������.
//	int objectSerialNum = 0;     //������Ʈ�� �̸��̶�� ���� ��
//	XMFLOAT4X4 departMat;         //������Ʈ�� ������ ��� 
//
//	MessageStruct() {}
//	MessageStruct(const MessageStruct& msg)
//	{
//		msgName = msg.msgName;
//		shaderName = msg.shaderName;
//		departMat = msg.departMat;
//		objectSerialNum = msg.objectSerialNum;
//	}
//};