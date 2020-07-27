#pragma once

#include "../stdafx.h"
#include "SingleTon/Singleton.h"
#include "../../Scenes/BaseScene.h"
#include "../../CShaders/Shader.h"
#include <WinSock2.h>
#include "../protocol.h"

constexpr int SERVER_PORT = 9000;
constexpr int BUF_SIZE = 1024;

#define WM_SOCKET WM_USER + 1


class CGameFramework;

class NetWorkManager : public Singleton<NetWorkManager>
{
private:
	int m_iRoomNum;
	bool m_bGameMode;
	string m_sPlayerName;

	SOCKET	sock;
	int		myId;
	WSABUF	send_wsabuf;
	char 	send_buffer[BUF_SIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUF_SIZE];
	char	packet_buffer[BUF_SIZE];
	DWORD	in_packet_size = 0;
	int		saved_packet_size = 0;

public:
	void SetCurScene(shared_ptr<BaseScene> curScene) { m_pCurScene = curScene; }
	void ResetCurScene() { m_pCurScene.reset(); }

	void SetRoomNum(const int& num) { m_iRoomNum = num; }
	void SetGameMode(const bool& mode) { m_bGameMode = mode; }
	void SetPlayerName(const string& name) { m_sPlayerName = name; }
	void SetNumPlayer(const int& num) { m_iNumPlayer = num; }

	void SetHwnd(const HWND& hwnd) { m_hWnd = hwnd; }

	int GetRoomNum() { return m_iRoomNum; }
	bool GetGameMode() { return m_bGameMode; }
	string GetPlayerName() { return m_sPlayerName; }
	int GetNumPlayer() { return m_iNumPlayer; }
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
	CS_PACKET_PLAYER_INFO* pInfo = NULL;

	HWND m_hWnd{ NULL };
	int m_iNumPlayer = 0;
private:
	//ReadPacket에서 받은 패킷들을 CGameFramework에 전달하기 위한 포인터
	//이거 프레임워크에다 전달하지 말고 curscene에다 전달
	shared_ptr<BaseScene> m_pCurScene;
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
	NetWorkManager();
	~NetWorkManager();
public:
	SOCKET getSock();
	void Initialize();
	void Release();
	void ConnectToServer();

	//Network클래스도 씬에접근하기 위해서에 접근가능하게 하기위해 내부 포인터를 갖고있게 함.
	void SetGameFrameworkPtr(HWND hWnd, shared_ptr<BaseScene> client);

	void ReadPacket();
	void ReadPacket(float fTimeElapsed);
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



	void SendReady();
	void SendNotReady();
	void SendReqStart();
	void SendReleaseKey();
	void SendPlayerInfoPacket();
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

};

