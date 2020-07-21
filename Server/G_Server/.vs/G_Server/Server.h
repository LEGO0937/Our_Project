#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "MyInclude.h"
#include "MyDefine.h"
#include "protocol.h"
#include "Terrain.h"

#pragma comment(lib, "Ws2_32.lib")
constexpr int MAX_BUFFER = 1024;

constexpr int SERVER_PORT = 9000;

constexpr float PLAYER_INIT_X_POS = 40;
constexpr float PLAYER_INIT_Y_POS = 0;
constexpr float PLAYER_INIT_Z_POS = 40;

constexpr float PLAYER_INIT_X_LOOK = 0;
constexpr float PLAYER_INIT_Y_LOOK = 0;
constexpr float PLAYER_INIT_Z_LOOK = 0;

constexpr float PLAYER_INIT_X_UP = 0;
constexpr float PLAYER_INIT_Y_UP = 0;
constexpr float PLAYER_INIT_Z_UP = 0;

constexpr float PLAYER_INIT_X_RIGHT = 0;
constexpr float PLAYER_INIT_Y_RIGHT = 0;
constexpr float PLAYER_INIT_Z_RIGHT = 0;

constexpr float FRICTION = 250;
constexpr float MAX_VELOCITY_XZ = 40;
constexpr float MAX_VELOCITY_Y = 400;


constexpr int MAX_FREEZE_COUNT = MAX_USER - 2;
constexpr int MAX_WORKER_THREAD = 6;

constexpr int GOLD_HAMMER_COUNT = 2;

enum EVENT_TYPE
{
	EV_RECV,
	EV_SEND,
	EV_COUNT,
	EV_BOMBERTOUCHCOOLTIME,
	EV_FREEZECOOLTIME,
	EV_GO_NEXTROUND,
	EV_GO_LOBBY
};
enum COLLISION_TYPE		//어느 객체와 충돌했는지 
{
	CL_NONE,
	CL_SURROUNDING,	//주변
	CL_PLAYER		//플레이어
};

enum GAME_STATE			//로비 상태인지 인게임 중인지 
{
	GS_ID_INPUT,
	GS_LOBBY,
	GS_INGAME
};
struct EVENT_ST
{
	int					obj_id;
	EVENT_TYPE			type;
	high_resolution_clock::time_point start_time;
	constexpr bool operator<(const EVENT_ST& left)const
	{
		return(type > left.type && start_time > left.start_time);
	}
};

// Overlapped구조체 확장
struct OVER_EX {
	WSAOVERLAPPED	over;
	WSABUF			dataBuffer;
	char			messageBuffer[MAX_BUFFER];
	EVENT_TYPE		event_t;
};

struct MAPOBJECT
{
	string name;
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
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

	XMFLOAT3 pos;
	XMFLOAT3 look;
	XMFLOAT3 up;
	XMFLOAT3 right;
	//XMFLOAT3 velocity;
	DWORD direction;
	//속도
	float fVelocity;

	XMFLOAT3 lastRightVector;
	XMFLOAT3 lastLookVector;
	XMFLOAT3 lastUpVector;


	COLLISION_TYPE collision;
	char score;
	char Item;
	char role;
	char matID;
	bool isFreeze;			//얼음 상태인지 여부
	bool isMoveRotate;		//애니메이션이 반복해서 동작하는 문제를 해결하기 위한 변수
	char nickname[32];
	//wchar_t nickname[12];
	char animation;			//애니메이션 index
	float animationTime;	//애니메이션 시간
	bool isReady;
	GAME_STATE gameState;

	short freezeCooltime;
	mutex freezeCooltime_l;

	// 부딪힌 오브젝트 인덱스
	short lastCollObjIdx;
	// 부딪힌 오브젝트 위치
	XMFLOAT3 lastCollPos;
	// 부딪힌 플레이어 인덱스 -> 안써서 주석처리
	//unsigned char lastCollPlayer;
	// 부딪힌 플레이어 위치
	XMFLOAT3 playerCollPos;

public:
	SOCKETINFO() {
		in_use = false;
		score = 0;
		Item = ITEM::EMPTY;
		matID = -1;
		isReady = false;
		freezeCooltime = 0;
		lastCollPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		playerCollPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		ZeroMemory(nickname, sizeof(wchar_t) * 12);
		ZeroMemory(&over_ex.messageBuffer, sizeof(over_ex.messageBuffer));
		ZeroMemory(&packet_buffer, sizeof(packet_buffer));
		over_ex.dataBuffer.len = MAX_BUFFER;
		over_ex.dataBuffer.buf = over_ex.messageBuffer;
		over_ex.event_t = EV_RECV;
	}
	void InitPlayer() {
		// in_use는 여기서 초기화 x
		matID = -1;
		isFreeze = false;
		isMoveRotate = false;
		isReady = false;
		score = 0;
		Item = ITEM::EMPTY;
	}
};

class Server
{
private:
	int round;
	SOCKET listenSocket;
	HANDLE iocp;
	// vector로 했을 때 over_ex.messagebuffer에 값이 들어오질 않는다.
	// 배열로 바꾸니 제대로 동작함. 왜? 무슨 차이?
	SOCKETINFO clients[MAX_USER];
	vector<thread> workerThreads;
	CHeightMapImage* heightMap;
	unsigned short roundCurrTime;
	int clientCount;
	int readyCount;
	int hostId;
	int bomberID;
	short bomberTouchCooltime;
	int freezeCnt;			//얼음 상태인 도망자 수  

	int goldTimerCnt[MAX_ROUND];
	int goldHammerCnt[MAX_ROUND];
	int normalHammerCnt[MAX_ROUND];

	int randomPosIdx[MAX_USER];

private:

	priority_queue <EVENT_ST> timer_queue;
	mutex		timer_l;
	mutex		roundTime_l;
	mutex		coolTime_l;
	mutex		freezeCnt_l;
	mutex		readyCnt_l;
	mutex		clientCnt_l;
	mutex		bomberID_l;

private:
	vector<MAPOBJECT> objects[MAX_ROUND];
	vector<MAPOBJECT> goldTimers[MAX_ROUND];
	vector<MAPOBJECT> goldHammers[MAX_ROUND];
	vector<MAPOBJECT> NormalHammers[MAX_ROUND];
	XMFLOAT3 spawn[MAX_ROUND][MAX_USER];

public:
	Server();
	~Server();
public:
	static void AcceptThread(LPVOID arg);
	void AcceptThreadFunc();
	static void WorkerThread(LPVOID arg);
	void WorkerThreadFunc();
	static void TimerThread(LPVOID arg);
	void TimerThreadFunc();
public:
	void ProcessPacket(char client, char* packet);
	void SendFunc(char client, void* packet);
	void RecvFunc(char client);
	void ClientDisconnect(char client);
public:
	void SendAccessComplete(char client);
	void SendAccessPlayer(char toClient, char fromClient);
	void SendClientLobbyIn(char toClient, char fromClient, char* name, const bool& isReady);
	void SendClientLobbyOut(char toClient, char fromClient);
	void SendChattinPacket(char to, char from, char* message);
	void SendPlayerAnimation(char toClient, char fromCllient);
	void SendPutPlayer(char toClient);
	void SendRoundStart(char client, unsigned short& time);
	void SendPleaseReady(char client);
	void SendReadyStatePacket(char toClient, char fromClient);
	void SendUnReadyStatePacket(char toClient, char fromClient);
	void SendMovePlayer(char to, char object);
	void SendRemovePlayer(char toClient, char fromClient);
	void SendRoundEnd(char client);
	void SendCompareTime(char client, unsigned short& time);
	void SendStopRunAnim(char toClient, char fromClient);
	void SendUseItem(char toClient, char fromClient, char useItem, char targetClient);
	void SendFreeze(char toClient, char fromClient);
	void SendReleaseFreeze(char toClient, char fromClient);
	void SendBombExplosion(char toClient, char fromClient);
	void SendChangeBomber(char toClient, char bomberId, char runnerId);
	void SendChangeHostID(char toClient, char hostID);
	void SendGetItem(char toClient, char fromClient, string& itemIndex);
	//void SendRoundScore(char client);
	void SendChoiceCharacter(char toClient, char fromClient, char matID);
	void SendChosenCharacter(char toClient);
	void SendGoLobby(char toClient);
	void SendFreezeCooltime(char toClient, char cooltime);
public:
	void SetAnimationState(char client, char animationNum);
	void SetVelocityZero(char client);
	void SetPitchYawRollZero(char client);
	void SetClient_Initialize(char client);
	void SetDirection(char client, int key);
	void RotateModel(char client, float x, float y, float z);
	void RotateClientAxisY(char client);
	void UpdateClientPos(char client);
	//void ProcessClientHeight(char client);
	//void ProcessFriction(char client, float& fLength);
	void ShuffleStartPosIndex();
public:
	void InitGame();
	void InitRound();
	int PickBomber();
	void StartTimer();
	void ResetTimer();

	void add_timer(int obj_id, EVENT_TYPE et, chrono::high_resolution_clock::time_point start_time);
public:
	bool InitServer();
	void RunServer();

public:
	void LoadMapObjectInfo();
	void LoadSpawnInfo();
public:
	void err_quit(const char*);
	void err_display(const char*);
};

