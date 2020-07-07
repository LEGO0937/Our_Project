#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include "../DinoRun/Common/stdafx.h"
#include "protocol.h"
#include <string>


constexpr int SERVER_PORT = 9000;
constexpr int BUF_SIZE = 1024;

class CGameFramework;

#ifdef _WITH_SERVER_
//싱글톤 

class Network : public Singleton<Network>
{
private:
	SOCKET	sock;
	int		myId;
	WSABUF	send_wsabuf;
	char 	send_buffer[BUF_SIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUF_SIZE];
	char	packet_buffer[BUF_SIZE];
	DWORD	in_packet_size = 0;
	int		saved_packet_size = 0;

private:
	CS_PACKET_UP_KEY* pUp = NULL;
	CS_PACKET_DOWN_KEY* pDown = NULL;
	CS_PACKET_RIGHT_KEY* pRight = NULL;
	CS_PACKET_LEFT_KEY* pLeft = NULL;
	CS_PACKET_READY* pReady = NULL;
	CS_PACKET_UNREADY* pUnReady = NULL;
	CS_PACKET_REQUEST_START* pRequestStart = NULL;
	CS_PACKET_RELEASE_KEY* pReleaseKey = NULL;
	CS_PACKET_ANIMATION* pAnimation = NULL;
	CS_PACKET_NICKNAME* pNickName = NULL;
	CS_PACKET_CHATTING* pText = NULL;
	CS_PACKET_OBJECT_COLLISION* pCollide = NULL;
	CS_PACKET_NOT_COLLISION* pNotCollide = NULL;
	CS_PACKET_PLAYER_COLLISION* pPlayerCollision = NULL;
	CS_PACKET_USE_ITEM* pItem = NULL;
	CS_PACKET_GET_ITEM* pGetItem = NULL;

	

private:
	//ReadPacket에서 받은 패킷들을 CGameFramework에 전달하기 위한 포인터
	CGameFramework* m_pGameClient{ nullptr };
	const  char* m_ServerIP{ nullptr };

public:
	void err_quit(const char* msg);
	void err_display(const char* msg);
	int GetMyID() { return myId; }
	void SetMyID(int id) { myId = id; }
	void SetServerIP(const char* ip) { m_ServerIP = ip; }
	const char* GetServerIP() { return m_ServerIP; }

	enum CONNECT_STATE { NONE, TRY, FAIL, OK };
	CONNECT_STATE GetConnectState()	const { return m_ConnectState; }
	void SetConnectState(CONNECT_STATE state) { m_ConnectState = state; }
public:
	CONNECT_STATE m_ConnectState;

public:
	Network();
	~Network();
public:
	SOCKET getSock();
	void Initialize();
	void ConnectToServer(HWND hWnd);

	//Network클래스도 CGameFramework에 접근가능하게 하기위해 내부 포인터를 갖고있게 함.
	void SetGameFrameworkPtr(HWND hWnd, CGameFramework* client);

	void ReadPacket();
	void SendPacket();
	void SendPacket(DWORD dataBytes);

public:
	void SendUpKey();
	void SendUpRightKey();
	void SendUpLeftKey();
	void SendDownKey();
	void SendDownRightKey();
	void SendDownLeftKey();
	void SendRightKey();
	void SendLeftKey();

	// 무슨 캐릭터 선택했는지 보냄
	void SendChoiceCharacter(int matID);

	void SendReady();
	void SendNotReady();
	void SendReqStart();
	void SendReleaseKey();
	void SendAnimationState(char animNum);
	void SendNickName(char id, _TCHAR* name);
	void SendChattingText(char id, const _TCHAR* text);
	void SendSurroundingCollision(USHORT objID);
	void SendNotCollision();
	void SendPlayerCollision(unsigned char playerID);
	void SendGetItem(const string& itemIndex);
	void SendUseItem(int useItem, int targetID);
	

public:
	CS_PACKET_REQUEST_START* GetRS() { return pRequestStart; }
	void SetNullRS() { pRequestStart = NULL; }
	/*SC_PACKET_ACCESS_COMPLETE* GetAC() { return pAC; }
	SC_PACKET_ACCESS_PLAYER* GetAP() { return pAP; }
	SC_PACKET_PUT_PLAYER* GetPP() {return pPP; }
	SC_PACKET_MOVE_PLAYER* GetMP() { return pMP; }
	SC_PACKET_USE_ITEM* GetUI() { return pUI; }
	SC_PACKET_ROLL_CHANGE* GetRC() { return pRC; }
	SC_PACKET_ROUND_END* GetRE() { return pRE; }
	SC_PACKET_REMOVE_PLAYER* GetRP() { return pRP; }
	SC_PACKET_COMPARE_TIME* GetCT() { return pCT; }
	SC_PACKET_ROUND_START* GetRS() { return pRS; }*/

};

#endif