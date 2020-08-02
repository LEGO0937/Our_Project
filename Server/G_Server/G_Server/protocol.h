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

enum PLAYER_NUM { P1, P2, P3, P4, P5, P6 };						// ��� �÷��̾� ���� 
enum STATE_TYPE { Init, Run, Over };

constexpr int SC_ACCESS_COMPLETE = 1;
constexpr int SC_PUT_PLAYER = 2;
constexpr int SC_MOVE_PLAYER = 3;
constexpr int SC_REMOVE_PLAYER = 4;
constexpr int SC_USE_ITEM = 5;
constexpr int SC_ROUND_END = 7;
constexpr int SC_ROUND_START = 8;
constexpr int SC_PLEASE_READY = 9;
constexpr int SC_ACCESS_PLAYER = 10;
constexpr int SC_COMPARE_TIME = 11;
constexpr int SC_STOP_RUN_ANIM = 12;
constexpr int SC_ANIMATION_INFO = 13;
constexpr int SC_CLIENT_LOBBY_IN = 14;
constexpr int SC_CLIENT_LOBBY_OUT = 15;
constexpr int SC_CHATTING = 16;
constexpr int SC_READY_STATE = 17;
constexpr int SC_UNREADY_STATE = 18;
constexpr int SC_PLAYER_INFO = 20;
constexpr int SC_GET_ITEM = 23;
//constexpr int SC_ROUND_SCORE = 24;
constexpr int SC_GO_LOBBY = 27;
constexpr int SC_EVENT = 28;



constexpr int CS_UP_KEY = 0;
constexpr int CS_DOWN_KEY = 1;
constexpr int CS_RIGHT_KEY = 2;
constexpr int CS_LEFT_KEY = 3;
constexpr int CS_UPLEFT_KEY = 4;
constexpr int CS_UPRIGHT_KEY = 5;
constexpr int CS_DOWNLEFT_KEY = 6;
constexpr int CS_DOWNRIGHT_KEY = 7;
constexpr int CS_READY = 8;
constexpr int CS_UNREADY = 9;
constexpr int CS_REQUEST_START = 10;
constexpr int CS_RELEASE_KEY = 11;
constexpr int CS_ANIMATION_INFO = 12;
constexpr int CS_NICKNAME_INFO = 13;
constexpr int CS_CHATTING = 14;
constexpr int CS_OBJECT_COLLISION = 15;
constexpr int CS_NOT_COLLISION = 16;
constexpr int CS_PLAYER_COLLISION = 17;
constexpr int CS_NOT_PLAYER_COLLISION = 18;
constexpr int CS_USEITEM = 19;
constexpr int CS_PLAYER_INFO = 20;
constexpr int CS_GET_ITEM = 24;
constexpr int CS_EVENT = 25;


struct MessageStruct
{
	char msgName;				//��ɾ�: ������Ʈ����, ���� or��ƼŬ �߰� or ��Ȱ��ȭ
	char shaderName;             //��� ���̴��� ��Ÿ���� ��������� global.h�� �� ����������.
	int objectSerialNum = 0;     //������Ʈ�� �̸��̶�� ���� ��
	XMFLOAT4X4 departMat;         //������Ʈ�� ������ ��� 

	MessageStruct() {}
	MessageStruct(const MessageStruct& msg)
	{
		msgName = msg.msgName;
		shaderName = msg.shaderName;
		departMat = msg.departMat;
		objectSerialNum = msg.objectSerialNum;
	}
};

//[Ŭ��->����]





//////////////////////////////////////////////////////

//<< InGame ��Ŷ ���� >>

//[Ŭ��->����]
struct CS_PACKET_PLAYER_INFO // Ŭ���� ��ġ, �̸�, üũ����Ʈ ���� ��
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
	char animation;			//�ִϸ��̼� ������ Ŭ�󿡼� �޾ƿ��� ��Ŷ
	char padding;			//4����Ʈ ������ ���� 
	//float animationTime;	//���� �ִϸ��̼� �ð�
};

struct CS_PACKET_RELEASE_KEY
{
	char size;
	char type;
};

// �÷��̾� �г��� ������ �뺸
struct CS_PACKET_NICKNAME
{
	char size;
	char type;
	char id;
	char padding;	//4����Ʈ ���������� 
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
//	unsigned short objId;		//object������ 66536�� ���� �ʱ� ������ unsigned short�� ����
//};

struct CS_PACKET_NOT_COLLISION
{
	char size;
	char type;
};


// ������ ���
struct CS_PACKET_EVENT
{
	char size;
	char type;
	char id;
	MessageStruct msg;
};

//////////////////////////////////////////////////////

//[����->Ŭ��]



// �� ���� ������ ���尡 ������ �� �� ���� ������ ��


//<< Ready Room ��Ŷ ���� >>
struct SC_PACKET_ACCESS_COMPLETE
{
	char size;
	char type;
	char myId;
	char serverTime;				// ���� �ð�
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

//������ Ŭ���̾�Ʈ�� ����
struct SC_PACKET_LOBBY_IN
{
	char size;
	char type;
	char id;
	clientsInfo client_state;
};

//������ Ŭ���̾�Ʈ�� ����
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



// �÷��̾� �̵� ��
struct SC_PACKET_PLAYER_INFO
{
	char size;
	char type;
	char id;
	UCHAR checkPoints;
	DWORD keyState;
	XMFLOAT4X4 xmf4x4Parents;
	string playerNames;
	// ������ ����, �߰�(���� �̸�)
	// ���� ���� �� �ο� ���ϰ� hmm...
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

//���� Ready���� �÷��̾��� ������ ���� ��Ŷ
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

// �÷��̾ ������ ��� ��
struct SC_PACKET_EVENT
{
	char id;
	char size;
	char type;

	MessageStruct msg;
};

// ���� �������� �����ð��� Ŭ��ð��� ���ϱ� ����
// �ٸ� ��� Ŭ��ð��� �����ð����� �缳��
struct SC_PACKET_COMPARE_TIME
{
	char size;
	char type;
	unsigned short serverTime;				// ���� �ð�
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



