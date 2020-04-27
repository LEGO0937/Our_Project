#pragma once
#include "Scene.h"
#include "protocol.h"
#include <string>
class Network
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
public:
	void err_quit(const char *msg);
	void err_display(const char *msg);
public:
	Network();
	~Network();
public:
	SOCKET getSock();
	void connectToServer(HWND hWnd);
	void ReadPacket(Scene *s);
	void SendPacket(int data);
};

