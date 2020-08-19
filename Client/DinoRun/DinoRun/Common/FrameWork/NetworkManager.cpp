
#include "NetworkManager.h"
#include "../Common/FrameWork/CGameFramework.h"
#include "../Scenes/StartScene.h"


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
	WSAAsyncSelect(sock, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);
}

void NetWorkManager::LoadToServer(HWND hWnd)
{
	switch (NetWorkManager::GetInstance()->GetConnectState())
	{
	case NetWorkManager::CONNECT_STATE::NONE:
	{
		
		break;
	}

	case NetWorkManager::CONNECT_STATE::TRY:
	{
		NetWorkManager::GetInstance()->ConnectToServer(hWnd);
		break;
	}

	case NetWorkManager::CONNECT_STATE::FAIL:
	{
		
		break;
	}

	case NetWorkManager::CONNECT_STATE::OK:
	{
		//g_State = GAMESTATE::LOGIN;
		break;
	}

	default:
		break;
	}
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

		// IP가 잘못된 경우
		else
		{
			m_ConnectState = CONNECT_STATE::FAIL;
			return;
		}
	}

	
	
	int result = send(sock, m_sPlayerName.c_str(), strlen(m_sPlayerName.c_str()), 0); // 아이디 받는 부분
	char buf[10];
	result = recv(sock, buf, 10, 0);
	buf[result] = '\0';

	if (strcmp(buf, "False") == 0)
	{
		//printf_s("DB에서 ID를 확인할 수 없습니다\n DB오류\n");
		exit(-1);
	}
	else if (strcmp(buf, "Overlap") == 0)
	{
		//printf_s("DB에서 ID를 확인할 수 없습니다\n DB접속\n");
		exit(-1);
	}
	else if (strcmp(buf, "Newid") == 0)
	{	
		//printf_s("아이디가 없습니다.\n ID를 만들어 게임을 시작하겠습니다.\n DB생성\n");
	}




	WSAAsyncSelect(sock, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ); // 작동하는거야

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
		{// 완성할 수 있을 때
			memcpy(packet_buffer + saved_packet_size, ptr, required);

			if (m_pCurScene)
			{
				m_pCurScene->ProcessPacket(packet_buffer, 0);
			}
			// 각 Scene의 ProcessPacket으로 처리를 넘김
			// ProcessPacket(packet_buffer);
			ptr += required;
			iobyte -= required;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else
		{// 완성 못 할 때
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
		{// 완성할 수 있을 때
			memcpy(packet_buffer + saved_packet_size, ptr, required);

			if (m_pCurScene)
			{
				m_pCurScene->ProcessPacket(packet_buffer, fTimeElapsed);
			}
			// 각 Scene의 ProcessPacket으로 처리를 넘김
			// ProcessPacket(packet_buffer);
			ptr += required;
			iobyte -= required;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else
		{// 완성 못 할 때
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}


//8바이트 이상일때는 이 SendPacket을 사용하여야한다.
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



// 아이템 -> 고기, 바위, 바나나, 진흙
// ItemColllision <- 아이템과 플레이어간의 충돌
// 리스트에 넣어났는디... 특정 아이템들 분류 시켰다라... 
// 아이템 저희 지금 클라에서 다루는게 플레이어 자신에 대해서만 처리하는 것인디 타 플레이어의 업뎃 같은거는 신경안쓰고 있는거
// 그르치?! 충돌때는 플레이어 자신의 처리만
// 아이템 같은 경우에는 충돌을 했잖아 아이템충돌





// Use아이템으로 사용했다는 신호와 동시에 사라지게 하는거야
void NetWorkManager::SendEvent(MessageStruct msg)
{
	pEvent = reinterpret_cast<CS_PACKET_EVENT*>(send_buffer);
	pEvent->msg = msg;
	pEvent->size = sizeof(pEvent);
	send_wsabuf.len = sizeof(pEvent);
	pEvent->type = CS_EVENT;

	SendPacket(pEvent->size);
}


// 소켓 함수 오류 출력 후 종료
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


// 소켓 함수 오류 출력
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


void NetWorkManager::SendColision()
{
	pAni = reinterpret_cast<CS_PACKET_PLAYER_ANI*>(send_buffer);
	pAni->size = sizeof(pAni);
	send_wsabuf.len = sizeof(pAni);
	pAni->type = CS_COLLISION_ANI;

	SendPacket();
}

void NetWorkManager::SendSliding()
{
	pAni = reinterpret_cast<CS_PACKET_PLAYER_ANI*>(send_buffer);
	pAni->size = sizeof(pAni);
	send_wsabuf.len = sizeof(pAni);
	pAni->type = CS_SLIDING_ANI;

	SendPacket();
}

void NetWorkManager::SendChangeGameMode()
{
	pGameMode = reinterpret_cast<CS_PACKET_GAME_MODE_INFO*>(send_buffer);
	pGameMode->size = sizeof(pGameMode);
	send_wsabuf.len = sizeof(pGameMode);
	pGameMode->type = CS_GAME_MODE_INFO;

	SendPacket();
}