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
enum PLAYER_NUM { P1, P2, P3, P4, P5, P6 };						// ��� �÷��̾� ���� 
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

//[Ŭ��->����]

struct SC_PACKET_ACCESS_COMPLETE
{
	char size;
	char type;
	char myId;
	char roundCount;		// �� ��������
	char serverTime;				// ���� �ð�
};

struct SC_PACKET_PUT_PLAYER
{
	char size;
	char type;
	char myId;
	char score;		// �÷��̾� ����
	char matID;			//������ ���ϴ� ĳ���ʹ� ���������� �ʿ��ϴ�.
	char xPos;		// ������Ʈ�� ��ġ
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

//<< InGame ��Ŷ ���� >>

//[Ŭ��->����]
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
	char touchedId;	// ��ġ�� �÷��̾� ��ȣ
};

//////////////////////////////////////////////////////

//[����->Ŭ��]
// ���� Ŭ��� ���� �ð����� �����������ϰ� �ִµ� ���� �ð��� �����Ӹ��� �����ִ°� ������?


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
	//char animNum;			// �ִϸ��̼� ��ȣ
	//char animTime;			// �ִϸ��̼� �ð� ����
	//char playerState;		// �÷��̾� ����

	//char usedItem;			// ���Ǵ� ������ ����
	//char roundCount;		// �� ��������
	//char timer;				// ���� �ð�

	
};

// �÷��̾� �̵� ��
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

// �÷��̾ ������ ��� ��
struct SC_PACKET_USE_ITEM
{
	char size;
	char type;
	char id;
	char usedItem;			// ���Ǵ� ������ ����
};

// ���� �������� �����ð��� Ŭ��ð��� ���ϱ� ����
// �ٸ� ��� Ŭ��ð��� �����ð����� �缳��
struct SC_PACKET_COMPARE_TIME
{
	char size;
	char type;
	char serverTime;				// ���� �ð�
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
	//XMFLOAT3 Pos; 	             // �÷��̾� ��ġ
	//XMFLOAT4 Dir;      	// ����(���ʹϾ�)
	//byte AnimationNum;    	// �ִϸ��̼� ��ȣ
	//float AnimationTime; 	// �ִϸ��̼� �ð� ����
	//byte UsedItem;      	// ���Ǵ� ������ ����
	//byte PlayerState;  	// �÷��̾� ����
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
