
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

	//Initialize();
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

void NetWorkManager::ConnectToServer()
{

	// connect()
	m_ServerIP = "127.0.0.1";
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(m_ServerIP);
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
				m_pCurScene->ProcessPacket(packet_buffer,0);
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

void NetWorkManager::SendPlayerInfoPacket()
{
	pInfo = reinterpret_cast<CS_PACKET_PLAYER_INFO*>(send_buffer);
	pInfo->size = sizeof(pInfo);
	send_wsabuf.len = sizeof(pInfo);
	pInfo->type = CS_PLAYER_INFO;

	SendPacket();
}

void NetWorkManager::SendUpKey()
{
	pUp = reinterpret_cast<CS_PACKET_UP_KEY*>(send_buffer);
	pUp->size = sizeof(pUp);
	send_wsabuf.len = sizeof(pUp);
	pUp->type = CS_UP_KEY;

	SendPacket();
}

void NetWorkManager::SendUpRightKey()
{
	pUp = reinterpret_cast<CS_PACKET_UP_KEY*>(send_buffer);
	pUp->size = sizeof(pUp);
	send_wsabuf.len = sizeof(pUp);
	pUp->type = CS_UPRIGHT_KEY;

	SendPacket();
}

void NetWorkManager::SendUpLeftKey()
{
	pUp = reinterpret_cast<CS_PACKET_UP_KEY*>(send_buffer);
	pUp->size = sizeof(pUp);
	send_wsabuf.len = sizeof(pUp);
	pUp->type = CS_UPLEFT_KEY;

	SendPacket();
}

void NetWorkManager::SendDownKey()
{
	pDown = reinterpret_cast<CS_PACKET_DOWN_KEY*>(send_buffer);
	pDown->size = sizeof(pDown);
	send_wsabuf.len = sizeof(pDown);
	pDown->type = CS_DOWN_KEY;

	SendPacket();
}

void NetWorkManager::SendDownRightKey()
{
	pDown = reinterpret_cast<CS_PACKET_DOWN_KEY*>(send_buffer);
	pDown->size = sizeof(pDown);
	send_wsabuf.len = sizeof(pDown);
	pDown->type = CS_DOWNRIGHT_KEY;

	SendPacket();
}

void NetWorkManager::SendDownLeftKey()
{
	pDown = reinterpret_cast<CS_PACKET_DOWN_KEY*>(send_buffer);
	pDown->size = sizeof(pDown);
	send_wsabuf.len = sizeof(pDown);
	pDown->type = CS_DOWNLEFT_KEY;

	SendPacket();
}

void NetWorkManager::SendRightKey()
{
	pRight = reinterpret_cast<CS_PACKET_RIGHT_KEY*>(send_buffer);
	pRight->size = sizeof(pRight);
	send_wsabuf.len = sizeof(pRight);
	pRight->type = CS_RIGHT_KEY;

	SendPacket();
}

void NetWorkManager::SendLeftKey()
{
	pLeft = reinterpret_cast<CS_PACKET_LEFT_KEY*>(send_buffer);
	pLeft->size = sizeof(pLeft);
	send_wsabuf.len = sizeof(pLeft);
	pLeft->type = CS_LEFT_KEY;

	SendPacket();
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

void NetWorkManager::SendAnimationState(char animNum)
{
	pAnimation = reinterpret_cast<CS_PACKET_ANIMATION*>(send_buffer);
	pAnimation->size = sizeof(pAnimation);
	send_wsabuf.len = sizeof(pAnimation);
	pAnimation->type = CS_ANIMATION_INFO;
	pAnimation->animation = animNum;

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

void NetWorkManager::SendSurroundingCollision(USHORT objID)
{
	pCollide = reinterpret_cast<CS_PACKET_OBJECT_COLLISION*>(send_buffer);
	pCollide->objId = objID;
	pCollide->size = sizeof(pCollide);
	send_wsabuf.len = sizeof(pCollide);
	pCollide->type = CS_OBJECT_COLLISION;

	SendPacket();
}


void NetWorkManager::SendNotCollision()
{
	pNotCollide = reinterpret_cast<CS_PACKET_NOT_COLLISION*>(send_buffer);
	pNotCollide->size = sizeof(pNotCollide);
	pNotCollide->type = CS_NOT_COLLISION;
	send_wsabuf.len = sizeof(pNotCollide);

	SendPacket();
}

void NetWorkManager::SendPlayerCollision(unsigned char playerID)
{
	pPlayerCollision = reinterpret_cast<CS_PACKET_PLAYER_COLLISION*>(send_buffer);
	pPlayerCollision->size = sizeof(pPlayerCollision);
	pPlayerCollision->type = CS_PLAYER_COLLISION;
	pPlayerCollision->playerID = playerID;		//충돌한 플레이어 ID
	send_wsabuf.len = sizeof(pPlayerCollision);

	SendPacket();
}

// 아이템 -> 고기, 바위, 바나나, 진흙
// ItemColllision <- 아이템과 플레이어간의 충돌
// 리스트에 넣어났는디... 특정 아이템들 분류 시켰다라... 
// 아이템 저희 지금 클라에서 다루는게 플레이어 자신에 대해서만 처리하는 것인디 타 플레이어의 업뎃 같은거는 신경안쓰고 있는거
// 그르치?! 충돌때는 플레이어 자신의 처리만
// 아이템 같은 경우에는 충돌을 했잖아 아이템충돌




void NetWorkManager::SendGetItem(const string& itemIndex)
{
	pGetItem = reinterpret_cast<CS_PACKET_GET_ITEM*>(send_buffer);
	pGetItem->size = sizeof(CS_PACKET_GET_ITEM);
	send_wsabuf.len = sizeof(CS_PACKET_GET_ITEM);
	pGetItem->type = CS_GET_ITEM;
	//for (int i = 0; i < 15; ++i)
	//	cout << pGetItem->itemIndex;
	// cout << "\n";
	ZeroMemory(pGetItem->itemIndex, MAX_ITEM_NAME_LENGTH);
	strncpy(pGetItem->itemIndex, itemIndex.c_str(), itemIndex.length());

	SendPacket();
}


void NetWorkManager::SendUseItem(int useItem, int targetID)
{
	pItem = reinterpret_cast<CS_PACKET_USE_ITEM*>(send_buffer);
	pItem->usedItem = useItem;
	pItem->target = targetID;
	pItem->size = sizeof(pItem);
	send_wsabuf.len = sizeof(pItem);
	pItem->type = CS_USEITEM;

	SendPacket();
}
// Use아이템으로 사용했다는 신호와 동시에 사라지게 하는거야



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

