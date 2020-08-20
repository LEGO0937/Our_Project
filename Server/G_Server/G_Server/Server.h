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

enum DB_Info
{
	DB_Success,
	DB_NoData,
	DB_NoConnect,
	DB_Overlap
};

// Overlapped구조체 확장
struct OVER_EX 
{
	WSAOVERLAPPED	over;
	WSABUF			dataBuffer;
	char			messageBuffer[MAX_BUFFER];
	EVENT_TYPE		event_t;
};


// 클라이언트
class SOCKETINFO
{
public:
	// in_use와 x,y에서 data race발생
	// in_use가 true인걸 확인하고 send하려고 하는데 그 이전에 접속 종료해서 false가된다면?
	// 그리고 send 전에 새 플레이어가 id를 재사용한다면? => 엉뚱한 곳에 send가 됨
	// mutex access_lock;
	bool isInGameReady = false; // 인게임에서 리소스 빌드가 완료됬는지 유무
	bool in_use; // 연결됬냐 연결 안됬냐
	OVER_EX over_ex;
	SOCKET socket;
	// 조립불가한 메모리를 다음번에 조립하기 위한 임시저장소
	char packet_buffer[MAX_BUFFER];
	int prev_size;
	string id;

	
	// 인게임용
	char rank; //등수
	char game_id[10]; // 인게임 아이디
	// char nickname[32]; // 이걸 굳이 받아야하나...? 나중에 상의 
	XMFLOAT4X4 xmf4x4Parents = {}; // 플레이어 위치값
	MessageStruct msg; // 애니메이션
	int checkPoints; // 인게임 내 체크포인트
	bool isReady; // 레디 or 언레디
	GAME_STATE gameState; // 현재 이 클라의 접속 상태
	DWORD keyState; // 이것도 애니메이션 부분
	XMFLOAT3 xmf3PutPos = {}; // 초기위치 지정이라는데...
	
	
	// 나중에 벡터로 바꾸든지 하자


	
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


// 방 생성정보(방 번호) 및 방 인원, 게임중, 아이템전 or 스피드전


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
	bool game_mode = false; // default = false

	

	// DB와 함께 사용
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
	void SendEventPacket(char toClient, const MessageStruct& msg); // 애니메이션 상태 패킷
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
//	char msgName;				//명령어: 오브젝트삭제, 생성 or파티클 추가 or 비활성화
//	char shaderName;             //담당 쉐이더를 나타내는 상수데이터 global.h에 값 정리돼있음.
//	int objectSerialNum = 0;     //오브젝트의 이름이라고 보면 됨
//	XMFLOAT4X4 departMat;         //오브젝트에 적용할 행렬 
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