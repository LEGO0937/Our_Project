#pragma once

// 최대 동접
constexpr int MAX_USER = 10;
// 맵크기
constexpr int WORLD_WIDTH = 8;
constexpr int WORLD_HEIGHT = 8;

constexpr int SERVER_PORT = 3500;

// define이 아닌 constexpr을 표준으로 사용하기로 함.
constexpr int CS_UP = 1;
constexpr int CS_DOWN = 2;
constexpr int CS_LEFT = 3;
constexpr int CS_RIGHT = 4;

// 접속 완료
constexpr int SC_LOGIN_OK = 1;
// 네 옆에 얘가 있으니 화면에 표시하라
constexpr int SC_PUT_PLAYER = 2;
// 이 플레이어가 접속 종료했으니 화면에 표시하지 말아라
constexpr int SC_REMOVE_PLAYER = 3;
// 이 플레이어가 위치를 변경했으니 수정하라
constexpr int SC_MOVE_PLAYER = 4;

#pragma pack(push, 1)
struct cs_packet_up {
	char size;
	char type;
};

struct cs_packet_down {
	char size;
	char type;
};

struct cs_packet_left {
	char size;
	char type;
};

struct cs_packet_right {
	char size;
	char type;
};

struct sc_packet_login_ok {
	char size;
	char type;
	char id;
};

struct sc_packet_put_player {
	char size;
	char type;
	char id;
	char x, y;
};

struct sc_packet_remove_player {
	char size;
	char type;
	char id;
};

struct sc_packet_move_player {
	char size;
	char type;
	char id;
	char x, y;
};
#pragma pack(pop)
