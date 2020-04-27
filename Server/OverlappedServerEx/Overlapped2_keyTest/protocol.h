#pragma once
#include <iostream>
#include <windows.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;
//<< Init 패킷 종류 >>

//[클라->서버]
struct CS_INIT_PACKET
{

};

///////////////////////////////////////////////////////

//[서버->클라] // 매라운드가 시작할 때마다 보내주는 패킷
struct SC_INIT_PACKET
{
	byte myId;
	XMFLOAT3 ObjPos;   // 오브젝트들 위치
	XMFLOAT4 Dir;      // 방향(쿼터니언)
	unsigned short Score;     // 플레이어 점수
};

//////////////////////////////////////////////////////

//<< InGame 패킷 종류 >>

//[클라->서버]
struct CS_INGAME_PACKET
{
	byte PlayerNum;   // 플레이어 번호
	byte Key;      // 키 버튼
};

//////////////////////////////////////////////////////

//[서버->클라]
struct SC_INGAME_PACKET
{
	byte id;
	PLAYER Player_Packet;
	byte RoundCount;      // 몇 라운드인지
	float Timer;      // 게임 시간
	bool Bomb;      // 폭탄이 터졌는지
};

enum ITEM { NONE = 0, HAMMER, GOLD_HAMMER, GOLD_TIMER, BOMB };
enum PLAYER_NUM { P1, P2, P3, P4, P5, P6 }; // 몇번 플레이어 인지 
enum PLAYER_ANIMATION_NUM {
	Idle, Jump, RunFast,
	RunBackWard, ATK3, Digging, Die2, RaiseHand, Ice, Victory, Aert, Slide
}; // 플레이어 애니메이션 번호
enum PLAYER_STATE { NONE, ICE, BREAK }; // 플레이어 상태
enum STATE_TYPE { Init, Run, Over };

struct PLAYER
{
	bool isBomber;        	 // 플레이어들의 역할
	XMFLOAT3 Pos; 	             // 플레이어 위치
	XMFLOAT4 Dir;      	// 방향(쿼터니언)
	byte AnimationNum;    	// 애니메이션 번호
	float AnimationTime; 	// 애니메이션 시간 정보
	byte UsedItem;      	// 사용되는 아이템 정보
	byte PlayerState;  	// 플레이어 상태
};


//////////////////////////////////////////////////////
