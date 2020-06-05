#pragma once

//#define SERVER_IP "172.30.1.1"
#define SERVER_IP "192.168.200.130"

using namespace std;

constexpr int MAX_USER = 6;
constexpr int MAX_ROUND = 3;

constexpr int MAX_ROUND_TIME = 60;
constexpr int MAX_ITEM_NAME_LENGTH = 16;
constexpr int MAX_CHATTING_LENGTH = 100;
constexpr int COOLTIME = 3;
constexpr float VELOCITY = 0.7f;
constexpr float ROTATE_RATE = 0.02f;

struct clientsInfo
{
	char    id;
	bool	isReady;
	char	name[32];
};



enum ROLE { RUNNER, BOMBER };
enum ITEM { NORMALHAMMER = 0, GOLD_HAMMER, GOLD_TIMER, EMPTY }; // 플레이어 아이템(바나나, 고기 등등... 어떤 아이템이 있는지 알아야 될 것 같은디...ㅠ)

enum PLAYER_NUM { P1, P2, P3, P4, P5, P6 };						// 몇번 플레이어 인지 
enum PLAYER_STATE { NONESTATE, ICE, BREAK };							// 플레이어 상태(일반 질주, 부스터, 바나나, 풀렸을 때)
enum STATE_TYPE { Init, Run, Over };
enum MATERIAL { PINK, BROWN, WHITE, BLACK, BLUE, PANDA, ICEMAT };

// 서버 클라 상태
constexpr int SC_ACCESS_COMPLETE = 1;
constexpr int SC_PUT_PLAYER = 2;
constexpr int SC_MOVE_PLAYER = 3;
constexpr int SC_REMOVE_PLAYER = 4;
constexpr int SC_USE_ITEM = 5;
constexpr int SC_ROLE_CHANGE = 6;
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
constexpr int SC_FREEZE = 19;
constexpr int SC_RELEASE_FREEZE = 20;
constexpr int SC_BOMB_EXPLOSION = 21;
constexpr int SC_CHANGE_HOST_ID = 22;
constexpr int SC_GET_ITEM = 23;
constexpr int SC_CHOICE_CHARACTER = 25;
constexpr int SC_CHOSEN_CHARACTER = 26;
constexpr int SC_GO_LOBBY = 27;
constexpr int SC_FREEZE_COOLTIME = 28;


// 클라 서버 상태
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
constexpr int CS_FREEZE = 20;
constexpr int CS_RELEASE_FREEZE = 21;
constexpr int CS_BOMB_EXPLOSION = 22;
constexpr int CS_BOMBER_TOUCH = 23;
constexpr int CS_GET_ITEM = 24;
constexpr int CS_CHOICE_CHARACTER = 25;



//[클라->서버]



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

struct CS_PACKET_FREEZE					//얼음변신 알림 패킷
{
	char size;
	char type;
};

struct CS_PACKET_RELEASE_FREEZE		//얼음 해제 알림 패킷
{
	char size;
	char type;
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

struct CS_PACKET_BOMBER_TOUCH
{
	char size;
	char type;
	char touchId;	// 터치한 플레이어 번호
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

struct CS_PACKET_OBJECT_COLLISION
{
	char size;
	char type;
	unsigned short objId;		//object개수는 66536을 넘지 않기 때문에 unsigned short로 변경
};

struct CS_PACKET_NOT_COLLISION
{
	char size;
	char type;
};
struct CS_PACKET_PLAYER_COLLISION
{
	char size;
	char type;
	unsigned char playerID;
};

struct CS_PACKET_GET_ITEM
{
	char size;
	char type;
	char itemIndex[MAX_ITEM_NAME_LENGTH];
};

struct CS_PACKET_USE_ITEM
{
	char size;
	char type;
	char target;			// 사용대상이 존재할 때
	char usedItem;			// 사용되는 아이템 정보
};

struct CS_PACKET_BOMB_EXPLOSION
{
	char size;
	char type;
};

struct CS_PACKET_CHOICE_CHARACTER
{
	char size;
	char type;
	char matID;
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
	//char isBoomed;			// 폭탄이 터졌는지

	// parameter 생성자
	/*SC_INGAME_PACKET(char _id, char _isBomber, char _xPos, char _yPos, char _zPos, char _xDir, char _yDir, char _zDir, char _wDir,
		char _animNum, char _animTime, char _usedItem, char _playerState, char _roundCount, char _timer, char _isBoomed) :
		id(_id), isBomber(_isBomber), xPos(_xPos), yPos(_yPos), zPos(_zPos), xDir(_xDir), yDir(_yDir), zDir(_zDir), wDir(_wDir),
		animNum(_animNum), animTime(_animTime), usedItem(_usedItem), playerState(_playerState), roundCount(_roundCount), timer(_timer),
		isBoomed(_isBoomed) {};*/
};

//<< Ready Room 패킷 종류 >>
struct SC_PACKET_ACCESS_COMPLETE
{
	char size;
	char type;
	char myId;
	char hostId;
	char score;				// 플레이어 점수
	char roundCount;		// 몇 라운드인지
	char serverTime;				// 서버 시간
};

struct SC_PACKET_ACCESS_PLAYER
{
	char size;
	char type;
	char id;
};

struct SC_PACKET_CHANGE_HOST
{
	char size;
	char type;
	char hostID;
};
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
	char bomberID;
	char goldTimerCnt;
	char goldHammerCnt;
	char hammerCnt;
	char round;
	unsigned short startTime;
};

struct SC_PACKET_PUT_PLAYER
{
	//4바이트 정렬을 할 필요가 있다.
	char size;
	char type;
	char posIdx[MAX_USER];
};

// 플레이어 이동 시
struct SC_PACKET_MOVE_PLAYER
{

	char size;
	char type;
	char id;
	char padding;		//4바이트 정렬

	float xPos;
	float yPos;
	float zPos;

	//캐릭터의 진행 방향
	float xLook;
	float yLook;
	float zLook;
	float xUp;
	float yUp;
	float zUp;
	float xRight;
	float yRight;
	float zRight;

	//속도
	float fVelocity;

	bool  isMoveRotate;
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
struct SC_PACKET_GET_ITEM
{
	char size;
	char type;
	char id;
	char itemIndex[MAX_ITEM_NAME_LENGTH];
};

struct SC_PACKET_USE_ITEM
{
	char size;
	char type;
	char id;
	char target;
	char usedItem;			// 사용되는 아이템 정보
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
	char hostId;
};

struct SC_PACKET_ROUND_END
{
	char size;
	char type;
	bool isWinner;
	char score[MAX_USER];
};
//
//struct SC_PACKET_ROUND_SCORE
//{
//	char size;
//	char type;
//	char score[MAX_USER];
//};

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

struct SC_PACKET_FREEZE
{
	char size;
	char type;
	char id;
};

struct SC_PACKET_RELEASE_FREEZE
{
	char size;
	char type;
	char id;
};

struct SC_PACKET_BOMB_EXPLOSION
{
	char size;
	char type;
	char bomberId;
};

struct SC_PACKET_ROLE_CHANGE
{
	char size;
	char type;
	char bomberId;
	char runnerId;
};

struct SC_PACKET_CHOICE_CHARACTER
{
	char size;
	char type;
	char id;
	char matID;
};

struct SC_PACKET_CHOSEN_CHARACTER
{
	char size;
	char type;
	char matID[MAX_USER];
};

struct SC_PACKET_FREEZE_COOLTIME
{
	char size;
	char type;
	char cooltime;
};
//////////////////////////////////////////////////////
