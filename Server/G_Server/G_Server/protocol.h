#pragma once
#include "../G_Server/MyInclude.h"
#include "DirectX.h"
#include <string>

using namespace std;

//#define SERVER_IP "172.30.1.1"
//#define SERVER_IP "192.168.200.130"
#define SERVER_IP "127.0.0.1"
constexpr int MAX_USER = 6;

constexpr int MAX_ROUND_TIME = 0;
constexpr int MAX_ITEM_NAME_LENGTH = 16;
constexpr int MAX_CHATTING_LENGTH = 100;
struct clientsInfo
{
	char    id;
	bool	isReady;
	char	name[32];
};

enum ITEM { MUD, ROCK, BANANA, EMPTY };

enum PLAYER_NUM { P1, P2, P3, P4, P5, P6 };						// 몇번 플레이어 인지 
enum STATE_TYPE { Init, Run, Over };

constexpr int SC_ACCESS_COMPLETE = 0; // 연결 됬다
constexpr int SC_PUT_PLAYER = 1; // 플레이어를 놓는다
constexpr int SC_PLAYER_INFO = 2; // 플레이어의 위치, 체크포인트, 애니메이션 등
constexpr int SC_REMOVE_PLAYER = 3; // 플레이어 삭제
constexpr int SC_ROUND_END = 4; // 라운드 끝
constexpr int SC_ROUND_START = 5; // 라운드 시작
constexpr int SC_PLEASE_READY = 6; // 레디좀 박아라
constexpr int SC_ACCESS_PLAYER = 7; // 플레이어 연결
constexpr int SC_CLIENT_LOBBY_IN = 8; // 클라 로비 들감
constexpr int SC_CLIENT_LOBBY_OUT = 9; // 클라 로비 나감
constexpr int SC_CHATTING = 10; // 채팅(졸작 끝나고 사용할지 모름)
constexpr int SC_READY_STATE = 11; // 레디
constexpr int SC_UNREADY_STATE = 12; // 언레디
constexpr int SC_GO_LOBBY = 13; // 로그인하고 로비 들감
constexpr int SC_EVENT = 14; // 플레이어 아이템 관리
constexpr int SC_COMPARE_TIME = 15; // 서버와 클라 시간 비교


constexpr int CS_READY = 0; // 레디
constexpr int CS_UNREADY = 1; // 언레디
constexpr int CS_REQUEST_START = 2; // 시작 요청
constexpr int CS_RELEASE_KEY = 3; // 키 누르기
constexpr int CS_NICKNAME_INFO = 4; // 닉넴 정보
constexpr int CS_CHATTING = 5; // 채팅
constexpr int CS_PLAYER_INFO = 6; // 플레이어 위치
constexpr int CS_EVENT = 7; // 플레이어 아이템



struct MessageStruct
{
	char msgName;				//명령어: 오브젝트삭제, 생성 or파티클 추가 or 비활성화
	char shaderName;             //담당 쉐이더를 나타내는 상수데이터 global.h에 값 정리돼있음.
	int objectSerialNum = 0;     //오브젝트의 이름이라고 보면 됨
	XMFLOAT4X4 departMat;         //오브젝트에 적용할 행렬 

	MessageStruct() {}
	MessageStruct(const MessageStruct& msg)
	{
		msgName = msg.msgName;
		shaderName = msg.shaderName;
		departMat = msg.departMat;
		objectSerialNum = msg.objectSerialNum;
	}
};

//[클라->서버]





//////////////////////////////////////////////////////

//<< InGame 패킷 종류 >>

//[클라->서버]
struct CS_PACKET_PLAYER_INFO // 클라의 위치, 이름, 체크포인트 상태 등
{
	char size;
	char type;
	char id;
	UCHAR checkPoints;
	DWORD keyState;
	XMFLOAT4X4 xmf4x4Parents;
	string playerNames;
};


struct CS_PACKET_READY
{
	char size;
	char type;
};

struct CS_PACKET_UNREADY
{
	char size;
	char type;
};

struct CS_PACKET_REQUEST_START
{
	char size;
	char type;
};

struct CS_PACKET_ANIMATION
{
	char size;
	char type;
	char animation;			//애니메이션 정보를 클라에서 받아오는 패킷
	char padding;			//4바이트 정렬을 위한 
	//float animationTime;	//현재 애니메이션 시간
};

struct CS_PACKET_RELEASE_KEY
{
	char size;
	char type;
};

// 플레이어 닉네임 서버에 통보
struct CS_PACKET_NICKNAME
{
	char size;
	char type;
	char id;
	char padding;	//4바이트 정렬을위한 
	char name[24];
};

struct CS_PACKET_CHATTING
{
	char size;
	char type;
	char id;
	char padding;
	char chatting[MAX_CHATTING_LENGTH];
};

//struct CS_PACKET_OBJECT_COLLISION
//{
//	char size;
//	char type;
//	unsigned short objId;		//object개수는 66536을 넘지 않기 때문에 unsigned short로 변경
//};

struct CS_PACKET_NOT_COLLISION
{
	char size;
	char type;
};


// 아이템 사용
struct CS_PACKET_EVENT
{
	char size;
	char type;
	char id;
	MessageStruct msg;
};

//////////////////////////////////////////////////////

//[서버->클라]



// 몇 라운드 인지는 라운드가 시작할 때 한 번만 보내도 됨


//<< Ready Room 패킷 종류 >>
struct SC_PACKET_PUT_PLAYER
{
	char size;
	char type;
	XMFLOAT3 xmf3PutPos;
};


struct SC_PACKET_ACCESS_COMPLETE
{
	char size;
	char type;
	char myId;
	char serverTime;				// 서버 시간
};

struct SC_PACKET_ACCESS_PLAYER
{
	char size;
	char type;
	char id;
};

//struct SC_PACKET_CHANGE_HOST
//{
//	char size;
//	char type;
//	char hostID;
//};

//입장한 클라이언트의 정보
struct SC_PACKET_LOBBY_IN
{
	char size;
	char type;
	char id;
	clientsInfo client_state;
};

//퇴장한 클라이언트의 정보
struct SC_PACKET_LOBBY_OUT
{
	char size;
	char type;
	char id;
};

struct SC_PACKET_PLEASE_READY
{
	char size;
	char type;
};

struct SC_PACKET_ROUND_START
{
	char size;
	char type;
	char clientCount;
	unsigned short startTime;
};



// 플레이어 이동 시
struct SC_PACKET_PLAYER_INFO
{
	char size;
	char type;
	char id;
	UCHAR checkPoints;
	DWORD keyState;
	XMFLOAT4X4 xmf4x4Parents;
	string playerNames;
	// 아이템 삭제, 추가(같은 이름)
	// 숫자 같은 건 부여 못하고 hmm...
};


struct SC_PACKET_PLAYER_ANIMATION
{
	char size;
	char type;
	char id;
	char animation;

};

struct SC_PACKET_STOP_RUN_ANIM
{
	char size;
	char type;
	char id;
};

//현재 Ready중인 플레이어의 정보를 담은 패킷
struct SC_PACKET_READY_STATE
{
	char size;
	char type;
	char id;
};

struct SC_PACKET_UNREADY_STATE
{
	char size;
	char type;
	char id;
};

struct SC_PACKET_CHATTING
{
	char size;
	char type;
	char id;
	char padding;
	char message[MAX_CHATTING_LENGTH];
};

// 플레이어가 아이템 사용 시
struct SC_PACKET_EVENT
{
	char id;
	char size;
	char type;

	MessageStruct msg;
};

// 일정 간격으로 서버시간과 클라시간을 비교하기 위해
// 다를 경우 클라시간을 서버시간으로 재설정
struct SC_PACKET_COMPARE_TIME
{
	char size;
	char type;
	unsigned short serverTime;				// 서버 시간
};

struct SC_PACKET_REMOVE_PLAYER
{
	char size;
	char type;
	char id;
};


struct SC_PACKET_ROUND_END
{
	char size;
	char type;
	bool isWinner;
	UCHAR checkPoints;
};


struct SC_PACKET_GO_LOBBY
{
	char size;
	char type;
};


struct SC_PACKET_COLLIDED
{
	char size;
	char type;
	char id;
};


struct SC_PACKET_NOT_COLLIDED
{
	char size;
	char type;
	char id;
};



