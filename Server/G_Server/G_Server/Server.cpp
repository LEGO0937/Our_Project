#include "Server.h"


Server::Server()
{
	//clients.reserve(MAX_USER);
	
	clientCount = 0;

	workerThreads.reserve(MAX_WORKER_THREAD);
}


Server::~Server()
{	
	
	//clients.clear();
}

bool Server::InitServer()
{
	setlocale(LC_ALL, "korean");

	clientCount = 0;
	hostId = -1;
	// Winsock Start - windock.dll �ε�
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		cout << "Error - Can not load 'winsock.dll' file\n";
		return false;
	}

	// 1. ���ϻ���											Overlapped�ҰŸ� ������ WSA_FLAG_OVERLAPPED
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		cout << "Error - Invalid socket\n";
		return false;
	}

	// �������� ��ü����
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 2. ���ϼ���
	// std�� bind�� ȣ��ǹǷ� ������ bind�� �ҷ��ֱ� ���� �տ� ::����
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		cout << "Error - Fail bind\n";
		// 6. ��������
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return false;
	}

	// 3. ���Ŵ�⿭����
	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		cout << "Error - Fail listen\n";
		// 6. ��������
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return false;
	}
	return true;
}

void Server::RunServer()
{
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	for (int i = 0; i < MAX_USER; ++i)
	{
		//SOCKETINFO tmpClient;
		clients[i].prev_size = 0;
		//clients[i].xmf4x4Parents
		//��İ� ���߿� �ֱ�
		
		//�̺κп� Ŭ������ �ʱ���ġ ��û�ϴ� ��� ������
		//clients.emplace_back(tmpClient);
		//printf("Create Client ID: %d, PrevSize: %d, xPos: %d, yPos: %d, zPos: %d, xDir: %d, yDir: %d, zDir: %d\n", i, clients[i].prev_size, clients[i].xPos, clients[i].yPos, clients[i].zPos, clients[i].xDir, clients[i].yDir, clients[i].zDir);
	}

	for (int i = 0; i < MAX_WORKER_THREAD; ++i)
		workerThreads.emplace_back(thread{ WorkerThread, (LPVOID)this });
	thread accpetThread{ AcceptThread, (LPVOID)this };
	accpetThread.join();
	for (auto& th : workerThreads)
		th.join();
}

void Server::AcceptThread(LPVOID arg)
{
	Server* pServer = static_cast<Server*>(arg);
	pServer->AcceptThreadFunc();
}

void Server::AcceptThreadFunc()
{
	SOCKADDR_IN clientAddr{};
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket{};
	DWORD flags;

	while (1)
	{
		// clientSocket�� �񵿱������ ����� ���ؼ��� listenSocket�� �񵿱���̾�� �Ѵ�.
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Error - Accept Failure\n";
			return;
		}

		// id�� 0~5���� ����� ����
		int new_id = -1;
		for (int i = 0; i < MAX_USER; ++i)
		{
			// i�� �ε����� �ϴ� ���� �� ���� �ִ°�?
			if (false == clients[i].in_use)
			{
				new_id = i;
				break;
			}
		}
		if (-1 == new_id)
		{
			cout << "MAX USER overflow\n";
			continue;
		}

		bool isStarted = false;
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;
			if (GS_INGAME == clients[i].gameState)
			{
				isStarted = true;
				break;
			}
		}
		if (true == isStarted)
		{
			cout << "Players aleady Start!\n";
			continue;
		}

		///////////////////////////////////// Ŭ���̾�Ʈ �ʱ�ȭ ���� ���� ��ġ /////////////////////////////////////
		clients[new_id].socket = clientSocket;
		if (-1 == hostId)
		{
			hostId = new_id;
			printf("���� ������ %d�Դϴ�.\n", hostId);
		}
		SetClient_Initialize(new_id);
		///////////////////////////////////// Ŭ���̾�Ʈ �ʱ�ȭ ���� ���� ��ġ /////////////////////////////////////
		ZeroMemory(&clients[new_id].over_ex.over, sizeof(clients[new_id].over_ex.over));

		flags = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), iocp, new_id, 0);

		clients[new_id].in_use = true;

		SendAcessComplete(new_id);
		printf("%d Ŭ���̾�Ʈ ���� �Ϸ�\n", new_id);
		// ���� �����鿡�� ���� ������ ������ ���
		for (int i = 0; i < MAX_USER; ++i)
			if (true == clients[i].in_use)
				SendPlayerInfo(i, new_id);

		// ó�� ������ ������ ���� ������ ���
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;
			if (i == new_id)
				continue;
			SendPlayerInfo(new_id, i);
		}
		clientCnt_l.lock();
		++clientCount;
		printf("%d Ŭ���̾�Ʈ ���� �Ϸ�, ���� Ŭ���̾�Ʈ ��: %d\n", new_id, clientCount);
		clientCnt_l.unlock();
		RecvFunc(new_id);
	}

	// 6-2. ���� ��������
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();
}


void Server::RecvFunc(char client)
{
	DWORD flags = 0;

	if (WSARecv(clients[client].socket, &clients[client].over_ex.dataBuffer, 1, NULL, &flags, &(clients[client].over_ex.over), 0))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << "Error - IO pending Failure\n";
			while (true);
		}
	}
	else {
		// �񵿱������ ���ư��� �ʰ� ��������� ���ư��ٴ� ����.
		cout << "Non Overlapped Recv return.\n";
		while (true);
	}
}

void Server::WorkerThread(LPVOID arg)
{
	Server* pServer = static_cast<Server*>(arg);
	pServer->WorkerThreadFunc();
}

void Server::WorkerThreadFunc()
{
	while (true)
	{
		DWORD io_byte;
		ULONGLONG l_key;
		// �������� �����͸� �Ѱ����
		OVER_EX* over_ex;

		int is_error = GetQueuedCompletionStatus(iocp, &io_byte, &l_key, reinterpret_cast<LPWSAOVERLAPPED*>(&over_ex), INFINITE);

		if (0 == is_error)
		{
			int err_no = WSAGetLastError();
			if (64 == err_no)
			{
				ClientDisconnect(l_key);
				continue;
			}
			else
				err_display("GQCS : ");
		}

		if (0 == io_byte)
		{
			ClientDisconnect(l_key);
			continue;
		}

		char key = static_cast<char>(l_key);
		if (EV_RECV == over_ex->event_t)
		{
			// RECV ó��
			wcout << "Packet from Client: " << (int)key << "\n";
			// ��Ŷ����
			// ���� ũ��
			int rest = io_byte;
			// ���� ����
			char* ptr = over_ex->messageBuffer;
			char packet_size = 0;

			// ��Ŷ ������ �˾Ƴ��� (�߹ݺ��� ����)
			if (0 < clients[key].prev_size)
				packet_size = clients[key].packet_buffer[0];

			while (0 < rest) {
				if (0 == packet_size) packet_size = ptr[0];	// ptr[0]�� ���ݺ��� ó���� ��Ŷ
				// ��Ŷ ó���Ϸ��� �󸶳� �� �޾ƾ� �ϴ°�?
				// ������ ���� �������� ���� ��Ŷ�� ���� �� ������ prev_size ���ֱ�
				int required = packet_size - clients[key].prev_size;
				if (required <= rest) {
					// ��Ŷ ���� �� �ִ� ���

					// �տ� ���ִ� �����Ͱ� ����Ǿ����� �� ������ �� �ڿ� ����ǵ��� prev_size�� �����ش�.
					memcpy(clients[key].packet_buffer + clients[key].prev_size, ptr, required);
					ProcessPacket(key, clients[key].packet_buffer);
					rest -= required;
					ptr += required;
					// �̹� ���Ʊ� ������ ���� ��Ŷ�� ó���� �� �ֵ��� 0
					packet_size = 0;
					clients[key].prev_size = 0;
				}
				else {
					// ��Ŷ ���� �� ���� ���
					memcpy(clients[key].packet_buffer + clients[key].prev_size, ptr, rest);
					rest = 0;
					clients[key].prev_size += rest;
				}
			}

			RecvFunc(key);
		}
		else if(EV_SEND == over_ex->event_t)
		{
			// SEND ó��
			delete over_ex;
		}
		else if (EV_GO_LOBBY == over_ex->event_t)
		{
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (false == clients[i].in_use)
					continue;
				clients[i].gameState = GS_LOBBY;
				SendGoLobby(i);
			}
		}
		else
		{
			cout << "Unknown Event : " << over_ex->event_t << "\n";
			delete over_ex;
			//while (true);
		}
	}
}

void Server::ProcessPacket(char client, char* packet)
{
	
	/*int x = clients[client].xPos;
	int y = clients[client].yPos;
	int z = clients[client].zPos;*/

	// 0���� ������, 1���� ��ŶŸ��
	// packet[0] packet[1] 

	DWORD tmpDir;

	switch (packet[1]) // 0��°�� ��Ŷ ������ 1������ ��Ŷ Ÿ��
	{
	case CS_UP_KEY:
		printf("Press UP Key ID: %d\n", client);
		break;
	case CS_DOWN_KEY:
		printf("Press DOWN Key ID: %d\n", client);
		break;
	case CS_LEFT_KEY:
		printf("Press LEFT Key ID: %d\n", client);
		break;
	case CS_RIGHT_KEY:
		printf("Press RIGHT Key ID: %d\n", client);
		break;
	case CS_PLAYER_INFO:
	{
		printf("Player Info Success: %d\n", client);
		// �� Ŭ�󿡰� ��ġ �ε��� ����
		/*for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;

			SendPutPlayer(i);
		}*/
		break;
	}
	case CS_READY:
	{
		// Ŭ�� ���ʹ����� F5���������� CS_READY ��Ŷ�� ���ƿ´ٸ� ++readyCount�� clientCount���� �����ϰ� �ǰ� 
		// �Ʒ� CS_REQUEST_START�ȿ� if(clientCount<= readyCount) ������ ���� �ʴ� ���� �߻�
		readyCnt_l.lock();
		clientCnt_l.lock();
		printf("��ü Ŭ�� ��: %d\n", clientCount);
		if (readyCount < clientCount - 1)
			++readyCount;
		printf("Ready�� Ŭ�� ��: %d\n", readyCount);
		clientCnt_l.unlock();
		readyCnt_l.unlock();


		clients[client].isReady = true;
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].in_use == true)
				SendReadyStatePacket(i, client);
		}
		break;
	}
	case CS_UNREADY:
	{
		readyCnt_l.lock();
		if (readyCount > 0)
			--readyCount;
		printf("Ready�� Ŭ�� ��: %d\n", readyCount);
		readyCnt_l.unlock();

		clients[client].isReady = false;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].in_use == true)
				SendUnReadyStatePacket(i, client);
		}
		break;
	}
	case CS_GET_ITEM:
	{
		CS_PACKET_GET_ITEM* p = reinterpret_cast<CS_PACKET_GET_ITEM*>(packet);
		break;
	}
	default:
		wcout << L"���ǵ��� ���� ��Ŷ ���� ����!!\n";
		while (true);
	}
	/*clients[client].xPos = x;
	clients[client].yPos = y;
	clients[client].zPos = z;*/
}



void Server::SendFunc(char client, void* packet)
{
	char* p = reinterpret_cast<char*>(packet);
	OVER_EX* ov = new OVER_EX;
	ov->dataBuffer.len = p[0];
	ov->dataBuffer.buf = ov->messageBuffer;
	//ov->is_recv = false; // �̺�Ʈ Ÿ������ ��������������
	ov->event_t = EV_SEND;
	memcpy(ov->messageBuffer, p, p[0]);
	ZeroMemory(&ov->over, sizeof(ov->over));
	int error = WSASend(clients[client].socket, &ov->dataBuffer, 1, 0, 0, &ov->over, NULL);
	if (0 != error)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << "Error - IO pending Failure\n";
			//while (true);
		}
	}
	else {
		// �񵿱������ ���ư��� �ʰ� ��������� ���ư��ٴ� ����.
		//cout << "Non Overlapped Send return.\n";
		//while (true);
	}
}

void Server::SetClient_Initialize(char client)
{
	clients[client].collision = CL_NONE;
}


void Server::SendAcessComplete(char client)
{
	SC_PACKET_ACCESS_COMPLETE packet;
	packet.myId = client;
	packet.size = sizeof(packet);
	packet.type = SC_ACCESS_COMPLETE;

	SendFunc(client, &packet);
}

void Server::SendGoLobby(char toClient)
{
	SC_PACKET_GO_LOBBY packet;

	packet.size = sizeof(packet);
	packet.type = SC_GO_LOBBY;

	SendFunc(toClient, &packet);
}

void Server::SendReadyStatePacket(char toClient, char fromClient)
{
	SC_PACKET_READY_STATE packet;

	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_READY_STATE;

	SendFunc(toClient, &packet);
}

void Server::SendUnReadyStatePacket(char toClient, char fromClient)
{
	SC_PACKET_UNREADY_STATE packet;

	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_UNREADY_STATE;

	SendFunc(toClient, &packet);
}


void Server::SendPlayerInfo(char toClient, char fromClient)
{
	SC_PACKET_PLAYER_INFO packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_PLAYER_INFO;

	SendFunc(toClient, &packet);
}

// �÷��̾� ������ ���...? 
//void Server::SendPutPlayer(char toClient, char fromClient)
//{
//	SC_PACKET_PUT_PLAYER packet;
//	//packet.myId = fromClient;
//	packet.size = sizeof(packet);
//	packet.type = SC_PUT_PLAYER;
//	//packet.xPos = clients[fromClient].xPos;
//	//packet.yPos = clients[fromClient].yPos;
//	//packet.zPos = clients[fromClient].zPos;
//
//	SendFunc(toClient, &packet);
//}

void Server::SendGetItem(char toClient, char fromClient, string& itemIndex)
{

	SC_PACKET_GET_ITEM packet;

	packet.id = fromClient;
	packet.size = sizeof(SC_PACKET_GET_ITEM);
	packet.type = SC_GET_ITEM;
	ZeroMemory(packet.itemIndex, MAX_ITEM_NAME_LENGTH);
	strncpy_s(packet.itemIndex, itemIndex.c_str(), itemIndex.length());
	SendFunc(toClient, &packet);
}

void Server::SendUseItem(char toClient, char fromClient, char usedItem)
{
	SC_PACKET_USE_ITEM packet;

	packet.id = fromClient;
	packet.usedItem = usedItem;
	packet.size = sizeof(packet);
	packet.type = SC_USE_ITEM;

	SendFunc(toClient, &packet);
}

void Server::SendEventPacket(char client, const MessageStruct& msg)
{
	SC_PACKET_EVENT packet;

	packet.id = client;
	packet.msg = msg;
	packet.size = sizeof(packet);
	packet.type = SC_EVENT;
	
	SendFunc(client, &packet);
}

void Server::SetAnimationState(char client, char animationNum)
{
	clients[client].animation = animationNum;
}

void Server::SendRemovePlayer(char toClient, char fromClient)
{
	SC_PACKET_REMOVE_PLAYER packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_PLAYER;

	SendFunc(toClient, &packet);
}

void Server::ClientDisconnect(char client)
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (false == clients[i].in_use)
			continue;
		if (i == client)
			continue;
		SendRemovePlayer(i, client);
	}
	closesocket(clients[client].socket);
	clients[client].in_use = false;
	printf("%d Ŭ���̾�Ʈ ���� ����\n", (int)client);
}



void Server::err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	//exit(1);
}

void Server::err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	cout << msg << (char*)lpMsgBuf << endl;

	LocalFree(lpMsgBuf);
}