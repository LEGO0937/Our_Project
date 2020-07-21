#pragma once
//#include <iostream>
//#include <windows.h>
//#include <DirectXMath.h>

using namespace std;
//using namespace DirectX;

//#define SERVER_IP "127.0.0.1"
#define SERVER_IP "192.168.204.68"
//#define SERVER_IP "119.195.232.145"


enum ITEM { EMPTY, BANANA, MUD, ROCK};
enum PLAYER_NUM { P1, P2, P3, P4, P5, P6 };						// 몇번 플레이어 인지 
enum STATE_TYPE { Init, Run, Over };

constexpr int SC_ACCESS_COMPLETE = 1;
constexpr int SC_PUT_PLAYER = 2;
constexpr int SC_MOVE_PLAYER = 3;
constexpr int SC_REMOVE_PLAYER = 4;
constexpr int SC_USE_ITEM = 5;
constexpr int SC_ROUND_END = 7;

constexpr int CS_UP_KEY = 0;
constexpr int CS_DOWN_KEY = 1;
constexpr int CS_RIGHT_KEY = 2;
constexpr int CS_LEFT_KEY = 3;

//[클라->서버]

struct SC_PACKET_ACCESS_COMPLETE
{
	char size;
	char type;
	char myId;
	char roundCount;		// 몇 라운드인지
	char serverTime;				// 서버 시간
};

struct SC_PACKET_PUT_PLAYER
{
	char size;
	char type;
	char myId;
	char score;		// 플레이어 점수
	char matID;			//유저가 원하는 캐릭터는 재질정보가 필요하다.
	char xPos;		// 오브젝트들 위치
	char yPos;
	char zPos;
	char xLook;
	char yLook;
	char zLook;
	char xUp;
	char yUp;
	char zUp;
	char xRight;
	char yRight;
	char zRight;
};

//////////////////////////////////////////////////////

//<< InGame 패킷 종류 >>

//[클라->서버]
struct CS_PACKET_RIGHT_KEY
{
	char size;
	char type;
};

struct CS_PACKET_LEFT_KEY
{
	char size;
	char type;
};

struct CS_PACKET_UP_KEY
{
	char size;
	char type;
};

struct CS_PACKET_DOWN_KEY
{
	char size;
	char type;
};

struct CS_PACKET_BOMBER_TOUCH
{
	char size;
	char type;
	char touchedId;	// 터치한 플레이어 번호
};

//////////////////////////////////////////////////////

//[서버->클라]
// 현재 클라는 서버 시간으로 프레임조정하고 있는데 서버 시간을 프레임마다 보내주는게 옳은가?


struct SC_PACKET_INGAME_PACKET
{
	//char id;
	//char isBomber;
	//char xPos;
	//char yPos;
	//char zPos;
	//char xDir;
	//char yDir;
	//char zDir;
	//char wDir;
	//
	//char animNum;			// 애니메이션 번호
	//char animTime;			// 애니메이션 시간 정보
	//char playerState;		// 플레이어 상태

	//char usedItem;			// 사용되는 아이템 정보
	//char roundCount;		// 몇 라운드인지
	//char timer;				// 서버 시간

	
};

// 플레이어 이동 시
struct SC_PACKET_MOVE_PLAYER
{
	char size;
	char type;
	char id;
	char xPos;
	char yPos;
	char zPos;
	char xLook;
	char yLook;
	char zLook;
	char xUp;
	char yUp;
	char zUp;
	char xRight;
	char yRight;
	char zRight;
};

// 플레이어가 아이템 사용 시
struct SC_PACKET_USE_ITEM
{
	char size;
	char type;
	char id;
	char usedItem;			// 사용되는 아이템 정보
};

// 일정 간격으로 서버시간과 클라시간을 비교하기 위해
// 다를 경우 클라시간을 서버시간으로 재설정
struct SC_PACKET_COMPARE_TIME
{
	char size;
	char type;
	char serverTime;				// 서버 시간
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
};

struct PLAYER
{
	//XMFLOAT3 Pos; 	             // 플레이어 위치
	//XMFLOAT4 Dir;      	// 방향(쿼터니언)
	//byte AnimationNum;    	// 애니메이션 번호
	//float AnimationTime; 	// 애니메이션 시간 정보
	//byte UsedItem;      	// 사용되는 아이템 정보
	//byte PlayerState;  	// 플레이어 상태
};

struct SC_PACKET_PLAYER_INFO
{
	XMFLOAT4X4 xmf4x4Parents[6];
	string playerNames[6];
	int checkPoints[6];

	char size;
	char type;
	char id;
};

//////////////////////////////////////////////////////
