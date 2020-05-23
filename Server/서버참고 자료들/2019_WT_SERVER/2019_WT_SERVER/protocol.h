#pragma once
#include <iostream>
//#include <windows.h>
//#include <DirectXMath.h>

using namespace std;
//using namespace DirectX;

enum ITEM { NONE = 0, HAMMER, GOLD_HAMMER, GOLD_TIMER, BOMB };
enum PLAYER_NUM { P1, P2, P3, P4, P5, P6 };						// 몇번 플레이어 인지 
enum PLAYER_STATE { NONE, ICE, BREAK };							// 플레이어 상태
enum STATE_TYPE { Init, Run, Over };

//[클라->서버]
struct CS_INIT_PACKET
{

};

///////////////////////////////////////////////////////

//[서버->클라] // 매라운드가 시작할 때마다 보내주는 패킷
//struct SC_INIT_PACKET
//{
//	char type;
//	char myId;
//	char xPos;		// 오브젝트들 위치
//	char yPos;
//	char zPos;
//	char xDir;		// 방향(쿼터니언)
//	char yDir;
//	char zDir;
//	char wDir;
//	char score;		// 플레이어 점수
//};

struct SC_ACCESS_COMPLETE
{
	char type;
	char myId;
	char score;				// 플레이어 점수
	char roundCount;		// 몇 라운드인지
	char serverTime;				// 서버 시간
};

struct SC_PUT_PLAYER
{
	char type;
	char myId;
	char xPos;		// 오브젝트들 위치
	char yPos;
	char zPos;
	char xDir;		// 방향(쿼터니언)
	char yDir;
	char zDir;
	char wDir;
	char score;		// 플레이어 점수
};

//////////////////////////////////////////////////////

//<< InGame 패킷 종류 >>

//[클라->서버]
struct CS_RIGHT_KEY
{
	char type;
	char id;		// 플레이어 번호
};

struct CS_LEFT_KEY
{
	char type;
	char id;		// 플레이어 번호
};

struct CS_UP_KEYINGA
{
	char type;
	char id;		// 플레이어 번호
};

struct CS_DOWN_KEY
{
	char type;
	char id;		// 플레이어 번호
};

struct CS_BOMBER_TOUCH
{
	char type;
	char id;
	char touchedId;	// 터치한 플레이어 번호
};

//////////////////////////////////////////////////////

//[서버->클라]
// 폭탄 넘겨주는거 어떻게 구현했는지에 따라 isBomber 변수가 담길 구조체 선정
// 현재 클라는 서버 시간으로 프레임조정하고 있는데 서버 시간을 프레임마다 보내주는게 옳은가?
// -> 건오빠한테 물어보고 timer 변수가 담길 구조체 선정

// isBomber는 키 입력 시 충돌체크하여 역할이 넘어가도록
// animNum 및 animTime은 각 클라가 알아서 하도록
// 몇 라운드 인지는 라운드가 시작할 때 한 번만 보내도 됨
// isBoomed는 게임 시간을 기반으로
struct SC_INGAME_PACKET
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
	//char isBoomed;			// 폭탄이 터졌는지

	// parameter 생성자
	/*SC_INGAME_PACKET(char _id, char _isBomber, char _xPos, char _yPos, char _zPos, char _xDir, char _yDir, char _zDir, char _wDir,
		char _animNum, char _animTime, char _usedItem, char _playerState, char _roundCount, char _timer, char _isBoomed) :
		id(_id), isBomber(_isBomber), xPos(_xPos), yPos(_yPos), zPos(_zPos), xDir(_xDir), yDir(_yDir), zDir(_zDir), wDir(_wDir),
		animNum(_animNum), animTime(_animTime), usedItem(_usedItem), playerState(_playerState), roundCount(_roundCount), timer(_timer),
		isBoomed(_isBoomed) {};*/
};

// 플레이어 이동 시
struct SC_MOVE_PLAYER
{
	char type;
	char id;
	char xPos;
	char yPos;
	char zPos;
	char xDir;
	char yDir;
	char zDir;
	char wDir;
};

// 플레이어가 아이템 사용 시
struct SC_USE_ITEM
{
	char type;
	char id;
	char usedItem;			// 사용되는 아이템 정보
};

// 일정 간격으로 서버시간과 클라시간을 비교하기 위해
// 다를 경우 클라시간을 서버시간으로 재설정
struct SC_COMPARE_TIME
{
	char type;
	char serverTime;				// 서버 시간
};

struct SC_ROLL_CHANGE
{
	char type;
	char bomberId;
	char normalId;
};

struct SC_ROUND_END
{
	char type;
};

struct PLAYER
{
	//bool isBomber;        	 // 플레이어들의 역할
	//XMFLOAT3 Pos; 	             // 플레이어 위치
	//XMFLOAT4 Dir;      	// 방향(쿼터니언)
	//byte AnimationNum;    	// 애니메이션 번호
	//float AnimationTime; 	// 애니메이션 시간 정보
	//byte UsedItem;      	// 사용되는 아이템 정보
	//byte PlayerState;  	// 플레이어 상태
};


//////////////////////////////////////////////////////
