
#include "NetworkManager.h"
#include "../Common/FrameWork/CGameFramework.h"


#pragma warning(disable : 4996)


//volatile bool g_LoginFinished;
//const char* g_serverIP = nullptr;


NetWorkManager::NetWorkManager()
{
	sock = NULL;
	myId = -1;
	in_packet_size = 0;
	saved_packet_size = 0;

	Initialize();
}


NetWorkManager::~NetWorkManager()
{
	//m_pGameClient = nullptr;
	m_pCurScene = nullptr;
}



void NetWorkManager::Initialize()
{

	m_ConnectState = CONNECT_STATE::NONE;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		WSACleanup();
		PostQuitMessage(0);
	}

	//socket
	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (sock == INVALID_SOCKET)
	{
		closesocket(sock);
		WSACleanup();
		err_quit("socket()");
		return;
	}
}

void NetWorkManager::Release()
{
	//WSAAsyncSelect(sock, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);
}

void NetWorkManager::ConnectToServer(HWND hWnd)
{
	// connect()
	m_ServerIP = "127.0.0.1";
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(NetWorkManager::GetInstance()->GetServerIP());
	serveraddr.sin_port = htons(SERVER_PORT);

	int retval = WSAConnect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);
	if (retval == SOCKET_ERROR)
	{
		retval = GetLastError();
		if (retval == WSAEWOULDBLOCK)
		{
			m_ConnectState = CONNECT_STATE::NONE;
			return;
		}

		// IP�� �߸��� ���
		else
		{
			m_ConnectState = CONNECT_STATE::FAIL;
			return;
		}
	}


	
	//printf("���� �ȵ鰡��?");
	WSAAsyncSelect(sock, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ); // �۵��ϴ°ž�

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;

	m_ConnectState = CONNECT_STATE::OK;
}


SOCKET NetWorkManager::getSock()
{
	return sock;
}


void NetWorkManager::ReadPacket()
{
	DWORD iobyte, ioflag = 0;

	int retval = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (retval)
		err_display("WSARecv()");

	BYTE* ptr = reinterpret_cast<BYTE*>(recv_buffer);

	while (0 != iobyte)
	{
		if (0 == in_packet_size)
			in_packet_size = ptr[0];

		int required = in_packet_size - saved_packet_size;

		if (iobyte + saved_packet_size >= in_packet_size)
		{// �ϼ��� �� ���� ��
			memcpy(packet_buffer + saved_packet_size, ptr, required);

			if (m_pCurScene)
			{
				m_pCurScene->ProcessPacket(packet_buffer, 0);
			}
			// �� Scene�� ProcessPacket���� ó���� �ѱ�
			// ProcessPacket(packet_buffer);
			ptr += required;
			iobyte -= required;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else
		{// �ϼ� �� �� ��
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}
void NetWorkManager::ReadPacket(float fTimeElapsed)
{

	DWORD iobyte, ioflag = 0;

	int retval = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (retval)
		err_display("WSARecv()");

	BYTE* ptr = reinterpret_cast<BYTE*>(recv_buffer);

	while (0 != iobyte)
	{
		if (0 == in_packet_size)
			in_packet_size = ptr[0];

		int required = in_packet_size - saved_packet_size;

		if (iobyte + saved_packet_size >= in_packet_size)
		{// �ϼ��� �� ���� ��
			memcpy(packet_buffer + saved_packet_size, ptr, required);

			if (m_pCurScene)
			{
				m_pCurScene->ProcessPacket(packet_buffer, fTimeElapsed);
			}
			// �� Scene�� ProcessPacket���� ó���� �ѱ�
			// ProcessPacket(packet_buffer);
			ptr += required;
			iobyte -= required;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else
		{// �ϼ� �� �� ��
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}


//8����Ʈ �̻��϶��� �� SendPacket�� ����Ͽ����Ѵ�.
void NetWorkManager::SendPacket(DWORD dataBytes)
{
	DWORD iobyte = 0;

	send_wsabuf.len = dataBytes;
	int retval = WSASend(sock, &send_wsabuf, 1, &dataBytes, 0, NULL, NULL);
	if (retval)
	{
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			err_display("WSASend()");
		}
	}
}


void NetWorkManager::SendPacket()
{
	DWORD iobyte = 0;

	int retval = WSASend(sock, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
	if (retval)
	{
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			err_display("WSASend()");
		}
	}
}


void NetWorkManager::SendPlayerInfo(int checkPoints, DWORD keyState, XMFLOAT4X4 xmf4x4Parents)
{
	pInfo = reinterpret_cast<CS_PACKET_PLAYER_INFO*>(send_buffer);
	pInfo->size = sizeof(pInfo);
	send_wsabuf.len = sizeof(pInfo);
	pInfo->type = CS_PLAYER_INFO;
	pInfo->checkPoints = checkPoints;
	pInfo->keyState = keyState;
	pInfo->xmf4x4Parents = xmf4x4Parents;


	SendPacket(pInfo->size);
}


void NetWorkManager::SendReady()
{
	pReady = reinterpret_cast<CS_PACKET_READY*>(send_buffer);
	pReady->size = sizeof(pReady);
	send_wsabuf.len = sizeof(pReady);
	pReady->type = CS_READY;

	SendPacket();
}

void NetWorkManager::SendNotReady()
{
	pUnReady = reinterpret_cast<CS_PACKET_UNREADY*>(send_buffer);

	pUnReady->size = sizeof(pUnReady);
	pUnReady->type = CS_UNREADY;
	send_wsabuf.len = sizeof(pUnReady);

	SendPacket();
}

void NetWorkManager::SendReqStart()
{
	pRequestStart = reinterpret_cast<CS_PACKET_REQUEST_START*>(send_buffer);
	pRequestStart->size = sizeof(pRequestStart);
	send_wsabuf.len = sizeof(pRequestStart);
	pRequestStart->type = CS_REQUEST_START;

	SendPacket();
}


void NetWorkManager::SendReleaseKey()
{
	pReleaseKey = reinterpret_cast<CS_PACKET_RELEASE_KEY*>(send_buffer);
	pReleaseKey->size = sizeof(pReleaseKey);
	send_wsabuf.len = sizeof(pReleaseKey);
	pReleaseKey->type = CS_RELEASE_KEY;

	SendPacket();
}

void NetWorkManager::SendChattingText(char id, const _TCHAR* text)
{
	pText = reinterpret_cast<CS_PACKET_CHATTING*>(send_buffer);
	pText->size = sizeof(CS_PACKET_CHATTING);
	pText->type = CS_CHATTING;
	pText->id = id;
	pText->padding = 0;

	int nLen = WideCharToMultiByte(CP_ACP, 0, text, -1, NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_ACP, 0, text, -1, pText->chatting, nLen, NULL, NULL);

	SendPacket(pText->size);
}



void NetWorkManager::SendNickName(char id, _TCHAR* name)
{
	pNickName = reinterpret_cast<CS_PACKET_NICKNAME*>(send_buffer);
	pNickName->size = sizeof(CS_PACKET_NICKNAME);
	pNickName->type = CS_NICKNAME_INFO;
	pNickName->id = id;
	pNickName->padding = 0;

	int nLen = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_ACP, 0, name, -1, pNickName->name, nLen, NULL, NULL);


	SendPacket(pNickName->size);
}

void NetWorkManager::SetGameFrameworkPtr(HWND hWnd, shared_ptr<BaseScene>client)
{
	if (client)
	{
		m_pCurScene = client;
	}
}



// ������ -> ���, ����, �ٳ���, ����
// ItemColllision <- �����۰� �÷��̾�� �浹
// ����Ʈ�� �־�µ�... Ư�� �����۵� �з� ���״ٶ�... 
// ������ ���� ���� Ŭ�󿡼� �ٷ�°� �÷��̾� �ڽſ� ���ؼ��� ó���ϴ� ���ε� Ÿ �÷��̾��� ���� �����Ŵ� �Ű�Ⱦ��� �ִ°�
// �׸�ġ?! �浹���� �÷��̾� �ڽ��� ó����
// ������ ���� ��쿡�� �浹�� ���ݾ� �������浹





// Use���������� ����ߴٴ� ��ȣ�� ���ÿ� ������� �ϴ°ž�
void NetWorkManager::SendEvent(MessageStruct& msg)
{
	pEvent = reinterpret_cast<CS_PACKET_EVENT*>(send_buffer);
	pEvent->msg = msg;
	pEvent->size = sizeof(pEvent);
	send_wsabuf.len = sizeof(pEvent);
	pEvent->type = CS_EVENT;

	SendPacket(pEvent->size);
}


// ���� �Լ� ���� ��� �� ����
void NetWorkManager::err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}


// ���� �Լ� ���� ���
void NetWorkManager::err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

