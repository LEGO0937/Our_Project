#include "Network.h"
#pragma warning(disable : 4996)

Network::Network()
{
	sock = NULL;
	myId = -1;
	in_packet_size = 0;
	saved_packet_size = 0;
}

Network::~Network()
{

}

void Network::connectToServer(HWND hWnd)
{
	// 서버 IP주소 입력받기
	std::string s;
	printf("서버IP입력 : ");
	std::cin >> s;
	const char *serverIp = s.c_str();

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		PostQuitMessage(0);

	// socket()
	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");


	// 넌블로킹 소켓으로 전환
	/*u_long on = 1;
	int retval = ioctlsocket(sock, FIONBIO, &on);
	if (retval == SOCKET_ERROR)
		err_quit("ioctlsocket()");*/

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(serverIp);
	serveraddr.sin_port = htons(SERVER_PORT);
	int retval = WSAConnect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);
	if (retval == SOCKET_ERROR)
	{
		if (GetLastError() != WSAEWOULDBLOCK)
			err_quit("connect()");
	}

	WSAAsyncSelect(sock, hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;
}

SOCKET Network::getSock()
{
	return sock;
}

void Network::ReadPacket(Scene* scene)
{
	DWORD iobyte, ioflag = 0;

	int retval = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (retval) {
		err_display("WSARecv()");
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte)
	{
		if (0 == in_packet_size)
			in_packet_size = ptr[0];

		int required = in_packet_size - saved_packet_size;

		if (iobyte + saved_packet_size >= in_packet_size)
		{// 완성할 수 있을 때
			memcpy(packet_buffer + saved_packet_size, ptr, required);
			// 각 Scene의 ProcessPacket으로 처리를 넘김
			scene->ProcessPacket(packet_buffer);
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

void Network::SendPacket(int wParam)
{
	cs_packet_up *packet = NULL;
	cs_packet_down *packet1 = NULL;
	cs_packet_right *packet2 = NULL;
	cs_packet_left *packet3 = NULL;
	switch (wParam)
	{
	case VK_UP:
		packet = reinterpret_cast<cs_packet_up *>(send_buffer);
		packet->size = sizeof(packet);
		send_wsabuf.len = sizeof(packet);
		packet->type = CS_UP;
		break;
	case VK_DOWN:
		packet1 = reinterpret_cast<cs_packet_down *>(send_buffer);
		packet1->size = sizeof(packet1);
		send_wsabuf.len = sizeof(packet1);
		packet1->type = CS_DOWN;
		break;
	case VK_RIGHT:
		packet2 = reinterpret_cast<cs_packet_right *>(send_buffer);
		packet2->size = sizeof(packet2);
		send_wsabuf.len = sizeof(packet2);
		packet2->type = CS_RIGHT;
		break;
	case VK_LEFT:
		packet3 = reinterpret_cast<cs_packet_left *>(send_buffer);
		packet3->size = sizeof(packet3);
		send_wsabuf.len = sizeof(packet3);
		packet3->type = CS_LEFT;
		break;
	}
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

// 소켓 함수 오류 출력 후 종료
void Network::err_quit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void Network::err_display(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}