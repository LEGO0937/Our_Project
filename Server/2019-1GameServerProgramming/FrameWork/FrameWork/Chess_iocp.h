#pragma once
#include "Scene.h"
#include "Sprite.h"
#include "Network.h"
#include <string>
#include <map>
#include <conio.h>

// 칸 크기
const int Width = 60;
const int Height = 60;

// 객체 구분을 위한 상수
const int PAWN = 0;
const int ROOK = 1;
const int KNIGHT = 2;
const int BISHOP = 3;
const int QUEEN = 4;
const int KING = 5;

struct POSITION {
	POSITION() :x(0), y(0) {}
	POSITION(int x1, int y1) :x(x1), y(y1) {}
	int x;
	int y;
};

struct CHESSBOARD {
	int index[WORLD_HEIGHT][WORLD_WIDTH] = { 0, };
	bool isFilled;
};

class Player {
private:
	POSITION pos;
	int pieceType;
	bool isDraw;
	Sprite* pSprite;
public:
	Player();
	Player(POSITION* p, int t, bool b);
	~Player();

	void MakeSprite();

	void getPos(POSITION* p) { *p = pos; }
	int getPieceType() { return pieceType; }
	bool getIsDraw() { return isDraw; }
	Sprite& getSprite() { return *pSprite; }

	void setPos(POSITION p) { pos = p; }
	void setPieceType(int t) { pieceType = t; }
	void setIsDraw(bool b) { isDraw = b; }
};

class Chess_iocp : public Scene
{
private:
	Network* net;
	Sprite * pBGSprite;			// 사용할 이미지 또는 애니메이션
	CHESSBOARD* pChessBoard;
	//Player * pBlack;
	std::map<int, Player> pWhite;
	//SOCKET sock;
	int myId;
	bool initComplete;
	/*WSABUF	send_wsabuf;
	char 	send_buffer[BUF_SIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUF_SIZE];
	char	packet_buffer[BUF_SIZE];
	DWORD		in_packet_size = 0;
	int		saved_packet_size = 0;*/
public:
	Chess_iocp();
	~Chess_iocp();
public:
	void Render(HDC* cDC);
	void MouseInput(int iMessage, int x, int y);
	void KeyboardInput(int iMessage, int wParam);
	void KeyboardCharInput(int wParam);
	void Enter();
	void Destroy();
	void Update();
	void ProcessPacket(char *ptr);
	void setNetwork(void* n);
};