#include "Chess_iocp.h"
#include "FWMain.h"
#include <winsock2.h>

const int BACKGROUND = 0;



Player::Player() : pos(0, 0), pieceType(PAWN), isDraw(false) {}

Player::Player(POSITION* p, int t, bool b) : pSprite(NULL), pos(*p), pieceType(t), isDraw(b) {}

Player::~Player()
{
	if (pSprite)
		SAFE_DELETE(pSprite);
}

void Player::MakeSprite()
{
	pSprite = new Sprite;

	pSprite->Entry(PAWN, "image/w_pawn.bmp", pos.x*Width, pos.y*Height);
	pSprite->Entry(ROOK, "image/w_rook.bmp", pos.x*Width, pos.y*Height);
	pSprite->Entry(KNIGHT, "image/w_knight.bmp", pos.x*Width, pos.y*Height);
	pSprite->Entry(BISHOP, "image/w_bishop.bmp", pos.x*Width, pos.y*Height);
	pSprite->Entry(QUEEN, "image/w_queen.bmp", pos.x*Width, pos.y*Height);
	pSprite->Entry(KING, "image/w_king.bmp", pos.x*Width, pos.y*Height);
}

Chess_iocp::Chess_iocp()
{
	pBGSprite = NULL;
	pChessBoard = NULL;
	myId = 1;
	initComplete = false;
	//pBlack = NULL;
}
Chess_iocp::~Chess_iocp()
{
	if (pBGSprite)
		SAFE_DELETE(pBGSprite);
	if (pChessBoard)
		SAFE_DELETE(pChessBoard);
	//if(pBlack)
	//	SAFE_DELETE(pBlack);
	pWhite.clear();
}

void Chess_iocp::Enter()
{
	////////////////////// 서버 접속 ////////////////////////
	//int retval;

	//// 서버 IP주소 입력받기
	//std::string s;
	//printf("서버IP입력 : ");
	//std::cin >> s;
	//const char *serverIp = s.c_str();

	//// 윈속 초기화
	//WSADATA wsa;
	//if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	//	PostQuitMessage(0);

	//// socket()
	//sock = socket(AF_INET, SOCK_STREAM, 0);
	//if (sock == INVALID_SOCKET) err_quit("socket()");


	//// 넌블로킹 소켓으로 전환
	//u_long on = 1;
	//retval = ioctlsocket(sock, FIONBIO, &on);
	//if (retval == SOCKET_ERROR)
	//	err_quit("ioctlsocket()");
	//
	//// connect()
	//SOCKADDR_IN serveraddr;
	//ZeroMemory(&serveraddr, sizeof(serveraddr));
	//serveraddr.sin_family = AF_INET;
	//serveraddr.sin_addr.s_addr = inet_addr(serverIp);
	//serveraddr.sin_port = htons(SERVERPORT);
	//retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	//if (retval == SOCKET_ERROR)
	//{
	//	if(GetLastError() != WSAEWOULDBLOCK)
	//		err_quit("connect()");
	//}
	//
	//while (initComplete != true);
	////////////////////// 서버 접속 ////////////////////////
	
	if (pBGSprite == NULL)
	{
		pBGSprite = new Sprite;
		pBGSprite->Entry(BACKGROUND, "image/chessboard100.bmp", 0, 0);			// 애니메이션에 필요한 이미지 로드
	}

	if (pChessBoard == NULL)
		pChessBoard = new CHESSBOARD;
	
	/*if (pBlack == NULL)
	{
		POSITION tmp(4, 3);
		pBlack = new Player(&tmp,PAWN,false);
		pBlack->getSprite().Entry(PAWN, "image/b_pawn.bmp", tmp.x*Width, tmp.y*Height);
		pBlack->getSprite().Entry(ROOK, "image/bw_rook.bmp", tmp.x*Width, tmp.y*Height);
		pBlack->getSprite().Entry(KNIGHT, "image/b_knight.bmp", tmp.x*Width, tmp.y*Height);
		pBlack->getSprite().Entry(BISHOP, "image/b_bishop.bmp", tmp.x*Width, tmp.y*Height);
		pBlack->getSprite().Entry(QUEEN, "image/b_queen.bmp", tmp.x*Width, tmp.y*Height);
		pBlack->getSprite().Entry(KING, "image/b_king.bmp", tmp.x*Width, tmp.y*Height);
	}*/
}
void Chess_iocp::Destroy()
{
	if (pBGSprite)
		SAFE_DELETE(pBGSprite);
	if (pChessBoard)
		SAFE_DELETE(pChessBoard);
	//if (pBlack)
	//	SAFE_DELETE(pBlack);
	pWhite.clear();
}
void Chess_iocp::Render(HDC* cDC)
{
	pBGSprite->Render(cDC, BACKGROUND);

	for(auto& d : pWhite)
		d.second.getSprite().Render(cDC, d.second.getPieceType(), (UINT)RGB(255, 0, 255));
	//pBlack->getSprite().Render(cDC, pWhite->getPieceType(), (UINT)RGB(255, 0, 255));
	
}

void Chess_iocp::Update()
{

}
void Chess_iocp::MouseInput(int iMessage, int x, int y)
{
	switch (iMessage)
	{
	case WM_LBUTTONDOWN:
		//printf("%d %d\n", x, y);
		break;
	}
}
void Chess_iocp::KeyboardInput(int iMessage, int wParam)
{
	if (iMessage == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_UP:
		case VK_DOWN:
		case VK_RIGHT:
		case VK_LEFT:
			net->SendPacket(wParam);
			break;
		}
	}
}
void Chess_iocp::KeyboardCharInput(int wParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	}
}

void Chess_iocp::ProcessPacket(char *ptr)
{
	Player tmpPlayer;
	POSITION myPos;
	POSITION tmpPos;
	sc_packet_login_ok *p = NULL;
	sc_packet_put_player *p1 = NULL;
	sc_packet_move_player *p2 = NULL;
	sc_packet_remove_player *p3 = NULL;
	int moveX = 0;
	int moveY = 0;
	int myX = 0;
	int myY = 0;
	switch (ptr[1])
	{
	case SC_LOGIN_OK:
		p = reinterpret_cast<sc_packet_login_ok *>(ptr);
		myId = p->id;
		initComplete = true;
		printf("Login OK! My ID : %d\n", p->id);
		break;
	case SC_PUT_PLAYER:
		p1 = reinterpret_cast<sc_packet_put_player *>(ptr);
		tmpPlayer.setPos(POSITION((p1->x), (p1->y)));
		tmpPlayer.setPieceType(PAWN);
		tmpPlayer.setIsDraw(true);
		pWhite.emplace(p1->id, tmpPlayer);
		pWhite[p1->id].MakeSprite();
		pWhite[p1->id].getSprite().setLocation(pWhite[p1->id].getPieceType(), pBGSprite->getX(BACKGROUND) + p1->x*Width, pBGSprite->getY(BACKGROUND) + p1->y*Height);
		printf("Put Player ID : %d, X: %d, Y: %d\n", p1->id, p1->x, p1->y);
		break;
	case SC_MOVE_PLAYER:
		p2 = reinterpret_cast<sc_packet_move_player *>(ptr);
		
		pWhite[p2->id].getPos(&myPos);
		
		// 변화량
		moveX = p2->x - myPos.x;
		moveY = p2->y - myPos.y;

		if (myId == p2->id)
		{
			// 배경스크롤
			int tmpX = pBGSprite->getX(BACKGROUND);
			int tmpY = pBGSprite->getY(BACKGROUND);
			
			pBGSprite->setLocation(BACKGROUND, tmpX - moveX*Width, tmpY - moveY*Height);
			pWhite[myId].setPos(POSITION(p2->x, p2->y));

			// 배경이 이동한만큼 other 플레이어들도 이동시켜주기
			for (auto& w : pWhite)
			{
				if (myId != w.first)
				{
					int otherX = w.second.getSprite().getX(w.second.getPieceType());
					int otherY = w.second.getSprite().getY(w.second.getPieceType());
					w.second.getSprite().setLocation(w.second.getPieceType(), otherX - moveX*Width, otherY - moveY*Height);
				}
			}
		}
		else
		{
			myX = pWhite[p2->id].getSprite().getX(pWhite[p2->id].getPieceType());
			myY = pWhite[p2->id].getSprite().getY(pWhite[p2->id].getPieceType());
			pWhite[p2->id].setPos(POSITION(p2->x, p2->y));
			pWhite[p2->id].getSprite().setLocation(pWhite[p2->id].getPieceType(), myX + moveX*Width, myY + moveY*Height);
		}
		pWhite[p2->id].getPos(&myPos);
		printf("Move Player ID : %d, X: %d, Y: %d\n", p2->id, myPos.x, myPos.y);
		break;
	case SC_REMOVE_PLAYER:
		p3 = reinterpret_cast<sc_packet_remove_player *>(ptr);
		printf("ClientID : %d 종료\n", p3->id);
		pWhite.erase(p3->id);
		break;
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void Chess_iocp::setNetwork(void* n)
{
	net = reinterpret_cast<Network *>(n);
}
