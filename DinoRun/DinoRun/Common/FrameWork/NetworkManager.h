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
	XMFLOAT3 m_xmf3Position;

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

	void SetPosition(const XMFLOAT3& position) { m_xmf3Position = position; }

	void SetHwnd(const HWND& hwnd) { m_hWnd = hwnd; }

	XMFLOAT3 GetPosition() { return m_xmf3Position; }
	int GetRoomNum() { return m_iRoomNum; }
	bool GetGameMode() { return m_bGameMode; }
	string GetPlayerName() { return m_sPlayerName; }
	int GetNumPlayer() { return m_iNumPlayer; }
private:
	CS_PACKET_READY* pReady = NULL; // ����
	CS_PACKET_UNREADY* pUnReady = NULL; // ���� ����
	CS_PACKET_REQUEST_START* pRequestStart = NULL; // �����ض�
	CS_PACKET_RELEASE_KEY* pReleaseKey = NULL; // 
	CS_PACKET_NICKNAME* pNickName = NULL;
	CS_PACKET_CHATTING* pText = NULL;
	CS_PACKET_PLAYER_INFO* pInfo = NULL; // �÷��̾� ��ġ, �ִϸ��̼�
	CS_PACKET_EVENT* pEvent = NULL;  // ������ 

	HWND m_hWnd{ NULL };
	int m_iNumPlayer = 0;
private:
	//ReadPacket���� ���� ��Ŷ���� CGameFramework�� �����ϱ� ���� ������
	//�̰� �����ӿ�ũ���� �������� ���� curscene���� ����
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
	void LoadToServer(HWND hWnd);
	void ConnectToServer(HWND hWnd);

	//NetworkŬ������ ���������ϱ� ���ؼ��� ���ٰ����ϰ� �ϱ����� ���� �����͸� �����ְ� ��.
	void SetGameFrameworkPtr(HWND hWnd, shared_ptr<BaseScene> client);

	void ReadPacket();
	void ReadPacket(float fTimeElapsed);
	void SendPacket();
	void SendPacket(DWORD dataBytes);

public:
	void SendReady(); // �������
	void SendNotReady(); // �������
	void SendReqStart(); // ���(����)
	void SendReleaseKey(); // �׳� Ȥ�ø���
	void SendPlayerInfo(int checkPoints, DWORD keyState, XMFLOAT4X4 xmf4x4Parents); // ������ ���Ӿ�, ���ǵ� ���Ӿ�



	void SendNickName(char id, _TCHAR* name); // �г��� ��(���߿� �ǳ�)
	void SendChattingText(char id, const _TCHAR* text); // ä��(���� ������ ��� ��)
	void SendEvent(MessageStruct msg); // ������ ���� ��Ŷ(������ ���� ��)

public:
	CS_PACKET_REQUEST_START* GetRS() { return pRequestStart; }
	void SetNullRS() { pRequestStart = NULL; }

};
