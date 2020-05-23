#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// 소켓 정보 저장을 위한 구조체와 변수
struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};

// 관리 중인 소켓의 개수
int nTotalSockets = 0;
// 소켓 정보 핸들 관리
SOCKETINFO *SocketInfoArray[WSA_MAXIMUM_WAIT_EVENTS];
// 이벤트 객체 핸들 관리
WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
// 멀티스레드 환경에서 위 두 배열을 보호하기 위해 필요
CRITICAL_SECTION cs;
int gNum = 0;

// 비동기 입출력 처리 함수
DWORD WINAPI WorkerThread(LPVOID arg);
// 소켓 관리 함수
BOOL AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);
// 오류 출력 함수
void err_quit(const char *msg);
void err_display(const char *msg);

int main(int argc, char *argv[])
{
	int retval;
	InitializeCriticalSection(&cs);

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// 더미(dummy) 이벤트 객체 생성
	// 특정 소켓과 짝짓지 않고 특별한 용도로 사용 (150행)
	WSAEVENT hEvent = WSACreateEvent();
	if (hEvent == WSA_INVALID_EVENT)
		err_quit("WSACreateEvent()");
	EventArray[nTotalSockets++] = hEvent;

	// 스레드 생성
	HANDLE hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
	if (hThread == NULL)
		return 1;
	CloseHandle(hThread);

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	DWORD recvbytes, flags;

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 소켓 정보 추가
		if (AddSocketInfo(client_sock) == FALSE)
		{
			closesocket(client_sock);
			printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트 번호 = %d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			continue;
		}

		// 비동기 입출력 시작 <- 'Accept로 연결된 <너>의 정보를 받기 시작한다' 선언
		SOCKETINFO *ptr = SocketInfoArray[nTotalSockets - 1];
		flags = 0;
		retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, NULL);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				err_display("WSARecv()");
				RemoveSocketInfo(nTotalSockets - 1);
				continue;
			}
		}

		// 소켓의 개수(nTotalSockets) 변화를 알림
		// EventArray[0]가 가리키는 더미 이벤트 객체를 신호 상태로 만든다.
		WSASetEvent(EventArray[0]);
		// WSAWaitForMultipleEvents() 함수가 대기 상태에서 리턴한다.
	}

	// 윈속 종료
	WSACleanup();
	DeleteCriticalSection(&cs);
	return 0;
}

// 비동기 입출력 처리 함수
DWORD WINAPI WorkerThread(LPVOID arg)
{
	int retval;

	while (1)
	{
		// 이벤트 객체 관찰
		DWORD index = WSAWaitForMultipleEvents(nTotalSockets, EventArray, FALSE, WSA_INFINITE, FALSE);
		if (index == WSA_WAIT_FAILED)
			continue;
		index -= WSA_WAIT_EVENT_0;
		WSAResetEvent(EventArray[index]);
		// index가 0이면 클라이언트가 접속하여 새로운 소켓 정보가 추가 되었다는 뜻
		if (index == 0)
			continue;	// 소켓의 총 갯수가 변경되었으므로 다시 돌아가 다른 이벤트 객체가 신호 상태가 되기를 기다린다.

		// 클라이언트 정보 얻기 (화면 출력을 위해)
		SOCKETINFO *ptr = SocketInfoArray[index];
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);

		// 비동기 입출력 결과 확인
		DWORD cbTransferred, flags;	// cbTransferred가 0이면 통신 상대가 접속 종료한 것.
		retval = WSAGetOverlappedResult(ptr->sock, &ptr->overlapped, &cbTransferred, FALSE, &flags);
		if (retval == FALSE || cbTransferred == 0)
		{
			RemoveSocketInfo(index);
			printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트 번호 = %d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			continue;
		}

		// 데이터 전송량 갱신. 소켓 정보 구조체를 참조하여 받은건지 보낸건지 판단 가능.
		if (ptr->recvbytes == 0)
		{// 받은 거
			ptr->recvbytes = cbTransferred;
			ptr->sendbytes = 0;
			// 받은 데이터 출력
			ptr->buf[ptr->recvbytes] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), ptr->buf);
		}
		else
		{// 보낸 거
			ptr->sendbytes += cbTransferred;
		}

		// 보낸 데이더가 받은 데이터보다 적으면, 아직 보내지 못한 데이터를 마저 보낸다.
		// WSASend()는 비동기적으로 작동 -> 실제로 보낸 데이터 수는 다음번 루프를 돌 때 확인 가능. (182행)
		if (ptr->recvbytes > ptr->sendbytes)
		{
			// 데이터 보내기
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->overlapped.hEvent = EventArray[index];
			ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
			ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

			DWORD sendbytes;
			retval = WSASend(ptr->sock, &ptr->wsabuf, 1, &sendbytes, 0, &ptr->overlapped, NULL);
			if (retval == SOCKET_ERROR) {
				if(WSAGetLastError() != WSA_IO_PENDING)
					err_display("WSASend()");
				continue;
			}
		}
		// 소켓 정보 중 받은 데이터 수를 초기화한 후 (206행) 도착한 데이터를 읽는다.
		// WSARecv()는 비동기적으로 작동 -> 실제로 받은 데이터 수는 다음번 루프를 돌 때 확인 가능. (173행)
		else
		{
			ptr->recvbytes = 0;

			// 데이터 받기
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->overlapped.hEvent = EventArray[index];
			ptr->wsabuf.buf = ptr->buf;
			ptr->wsabuf.len = BUFSIZE;

			DWORD recvbytes;
			flags = 0;
			retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recvbytes, &flags, &ptr->overlapped, NULL);
			if (retval == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
					err_display("WSARecv()");
				continue;
			}
		}
	}
}

// 소켓 정보 추가
BOOL AddSocketInfo(SOCKET sock)
{
	EnterCriticalSection(&cs);
	if (nTotalSockets >= WSA_MAXIMUM_WAIT_EVENTS)
		return FALSE;

	SOCKETINFO *ptr = new SOCKETINFO;
	if (ptr == NULL)
		return FALSE;

	WSAEVENT hEvent = WSACreateEvent();
	if (hEvent == WSA_INVALID_EVENT)
		return FALSE;

	ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
	ptr->overlapped.hEvent = hEvent;
	ptr->sock = sock;
	ptr->recvbytes = ptr->sendbytes = 0;
	ptr->wsabuf.buf = ptr->buf;
	ptr->wsabuf.len = BUFSIZE;
	SocketInfoArray[nTotalSockets] = ptr;
	EventArray[nTotalSockets] = hEvent;
	++nTotalSockets;

	LeaveCriticalSection(&cs);
	return TRUE;
}

// 소켓 정보 삭제
void RemoveSocketInfo(int nIndex)
{
	EnterCriticalSection(&cs);

	SOCKETINFO *ptr = SocketInfoArray[nIndex];
	closesocket(ptr->sock);
	delete ptr;
	WSACloseEvent(EventArray[nIndex]);
	if (nIndex != (nTotalSockets - 1))
	{
		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets - 1];
		EventArray[nIndex] = EventArray[nTotalSockets - 1];
	}
	--nTotalSockets;

	LeaveCriticalSection(&cs);
}

// 소켓 함수 오류 출력 후 종료
void err_quit(const char *msg)
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
void err_display(const char *msg)
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