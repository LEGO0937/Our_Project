#include "Server.h"
#include "string"

using namespace std;

bool isStarted = false;

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
	// �ٷ� DB����
	Server::init_DB();

	clientCount = 0;
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
		// cout << "����11" << endl;
		// id�� 0~5���� ����� ����
		// ��.. �� ���̵� 1���� ����Ѵٰ�...?
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
	

		//-----------------------DB ó���κ�-----------------------------
		char  buf[255];
		int retval = recv(clientSocket, buf, 10, 0);
		if (retval == SOCKET_ERROR) {
			std::cout << "Not Recv Game_ID : " << new_id << std::endl;
			closesocket(clientSocket);
			continue;
		}
		buf[retval] = '\0';

		strcpy(clients[new_id].game_id, buf);
		printf_s("DBó������ Game ID: %s\n", clients[new_id].game_id);
		

		int is_accept = get_DB_Info(new_id);
		if (is_accept == DB_Success && db_connect == 0) {
			cout << "DB�� ����\n" << endl;
			strcpy(buf, "Okay");
			retval = send(clientSocket, buf, strlen(buf), 0);
		}
		else if (is_accept == DB_NoConnect) {
			cout << "DB����ȵ�\n" << endl;
			strcpy(buf, "False");
			retval = send(clientSocket, buf, strlen(buf), 0);
		}
		else if (db_connect == 1) {
			strcpy(buf, "Overlap");
			retval = send(clientSocket, buf, strlen(buf), 0);
			closesocket(clientSocket);
			continue;
		}
		else if (is_accept == DB_NoData) {
			cout << "DB���� ������\n" << endl;
			//cout << new_id << endl;
			strcpy(buf, "Newid");

			new_DB_Id(new_id);
			get_DB_Info(new_id);
			retval = send(clientSocket, buf, strlen(buf), 0);
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		
		
		// ���� ���� ���̵� init ó��

		///////////////////////////////////// Ŭ���̾�Ʈ �ʱ�ȭ ���� ���� ��ġ /////////////////////////////////////
		clients[new_id].socket = clientSocket;

		SetClient_Initialize(new_id);
		///////////////////////////////////// Ŭ���̾�Ʈ �ʱ�ȭ ���� ���� ��ġ /////////////////////////////////////

		

		flags = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), iocp, new_id, 0);

		clients[new_id].in_use = true;
		SendAcessComplete(new_id);
		SendGameMode(new_id);

		printf("%d Ŭ���̾�Ʈ ���� �Ϸ�\n", new_id);
		// ���� �����鿡�� ���� ������ ������ ���
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (true == clients[i].in_use)
			{
				SendAccessPlayer(i, new_id);
			}
		}


		// ó�� ������ ������ ���� ������ ���
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;
			/*if (i == new_id)
				continue;*/

			SendAccessPlayer(new_id, i);
			SendResetRoomInfo(i);
			SendRoomInfo(i);
			// ����� �� ��� ������Ʈ �߰��� ����!
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
			//while (true);
		}
	}
	else {
		// �񵿱������ ���ư��� �ʰ� ��������� ���ư��ٴ� ����.
		cout << "Non Overlapped Recv return.\n";
		//while (true);
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
			// cout << "Packet from Client: " << (int)key << "\n";
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
	// 0���� ������, 1���� ��ŶŸ��
	// packet[0] packet[1] 

	DWORD tmpDir;
	bool isInGameReady = false;
	switch (packet[1]) // 0��°�� ��Ŷ ������ 1������ ��Ŷ Ÿ��
	{
	case CS_PLAYER_INFO: // �÷��̾� ��ü���� ��ġ ���
	{
		//printf("Player Info Success: %d\n", client);
		// �� Ŭ�󿡰� ��ġ �ε��� ����
		CS_PACKET_PLAYER_INFO* p = reinterpret_cast<CS_PACKET_PLAYER_INFO*>(packet);	
		clients[client].checkPoints = p->checkPoints;
		clients[client].xmf4x4Parents = p->xmf4x4Parents;
		clients[client].keyState = p->keyState;
		//clients[client].id = p->playerNames;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if(clients[i].in_use == true)
				SendPlayerInfo(i, client);
		}
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
			if (false == clients[i].in_use)
				continue;
			/*if (i == new_id)
				continue;*/

			SendResetRoomInfo(i);
			SendRoomInfo(i);
			// ����� �� ��� ������Ʈ �߰��� ����!
		}
		//for (int i = 0; i < MAX_USER; ++i)
		//{
		//	if (clients[i].in_use == true)
		//		SendReadyStatePacket(i, client);
		//}
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
			if (false == clients[i].in_use)
				continue;
			/*if (i == new_id)
				continue;*/

			
			SendResetRoomInfo(i);
			SendRoomInfo(i);
			// ����� �� ��� ������Ʈ �߰��� ����!
		}
		//for (int i = 0; i < MAX_USER; ++i)
		//{
		//	if (clients[i].in_use == true)
		//		SendUnReadyStatePacket(i, client);
		//}
		break;
	}
	case CS_EVENT:
	{
		CS_PACKET_EVENT* p = reinterpret_cast<CS_PACKET_EVENT*>(packet);
		//clients[client].msg = p->msg;
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;
			else
			{
				SendEventPacket(i, p->msg);
			}
		}
		break;
	}
	case CS_REQUEST_START:
	{
		clientCnt_l.lock();
		readyCnt_l.lock();
		if (clientCount - 1 == readyCount)
		{
			//cout << clientCount << ", " << readyCount << "\n";
			readyCnt_l.unlock();
			clientCnt_l.unlock();


			// �� �ּ��κп� �ʱ� ��ġ ����

		/*	for (int i = 0; i < MAX_USER; ++i)
			{
				if (clients[i].in_use == true)
				{
					clients[i].gameState = GS_INGAME;
				}
			}*/


			// �������� ������ ������ġ�ε����� �ش��ϴ� ��ġ ����
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (false == clients[i].in_use)
					continue;
				
			}

			// �� Ŭ�󿡰� ������ġ �ε��� ����
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (false == clients[i].in_use)
					continue;

				SendRoundStart(i);
				SendPutPlayer(i);
			}
			//add_timer(-1, EV_COUNT, chrono::high_resolution_clock::now() + 1s);


			printf("Round Start\n");
		}
		else
		{
			readyCnt_l.unlock();
			clientCnt_l.unlock();
			////�� �κ� READYCOUNT ����
			//for (int i = 0; i < MAX_USER; ++i)
			//{
			//	if (true == clients[i].in_use)
			//		SendPleaseReady(i);
			//}
			// ���߿� ä�� ���� �����Ǹ� ����
			printf("Please Ready\n");
		}
		break;
	}
	case CS_GAME_MODE_INFO:
	{
		game_mode = !game_mode;
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].in_use == false)
				continue;
			else
				SendGameMode(i);
				//continue;
		}
		break;
	}
	case CS_SLIDING_ANI:
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].in_use == false)
				continue;
			else
				SendPlayerAni_Sliding(i, client);
			//continue;
		}
		break;
	case CS_COLLISION_ANI:
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].in_use == false)
				continue;
			else
				SendPlayerAni_Collision(i, client);
			//continue;
		}
		break;
	case CS_INGAME_READY:
		isInGameReady = false;
		clients[client].isInGameReady = true;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].in_use == false)
				continue;
			else
				if (!clients[i].isInGameReady)
				{
					isInGameReady = true;
					break;
				}
			//continue;
		}
		if (isInGameReady)
			break;
		for (int i = 0; i < MAX_USER; ++i)
		{
			SendInGameReady(i);
		}
		break;
	case CS_INGAME_FINISH:
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].in_use == false)
				continue;
			else
			{
				clients[i].isInGameReady = false;
				clients[i].isReady = false;
				
				SendInGameFinish(i, clients[client].game_id);
				
			}
		}
		isStarted = false;
		break;
	case CS_REMOVE_PLAYER:
		/*clients[client].in_use = false;
		clients[client].isReady = false;
		clients[client].game_id = ;*/
		clients[client].Init();
		/*SendRemovePlayer(client);
		
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].in_use == false)
				continue;
			SendResetRoomInfo(i);
			SendRoomInfo(i);
		}*/
		ClientDisconnect(client);

		break;
	default:
		wcout << L"��Ŷ ������: " << (int)packet[0] << endl;
		wcout << L"��Ŷ Ÿ�� : " << (int)packet[1] << endl;
		cout << "Client prev size : " << clients[client].prev_size << "\n";
		for (int i = 2; i < (int)packet[0]; ++i)
			wcout << packet[i];
		wcout << L"\n";
		wcout << L"���ǵ��� ���� ��Ŷ ���� ����!!\n";
		break;
	}
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
	std::cout << "id : " << client << std::endl;
	
	/*
	clients[new_id].in_use = true;
	clients[new_id].gameState = GS_ID_INPUT;
	*/
}

void Server::SendRoundStart(char client)
{
	SC_PACKET_ROUND_START packet;
	clientCnt_l.lock();
	packet.clientCount = clientCount;
	clientCnt_l.unlock();
	packet.size = sizeof(packet);
	packet.type = SC_ROUND_START;
	SendFunc(client, &packet);
}

void Server::SendRoomInfo(char client)
{
	SC_PACKET_USERS_INFO packet;
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (false == clients[i].in_use)
			continue;

		else
		{
			packet.users.m_sName = clients[i].game_id;
			packet.users.m_bReadyState = clients[i].isReady;
		}


		packet.size = sizeof(SC_PACKET_USERS_INFO);
		packet.type = SC_ROOM_INFO;

		SendFunc(client, &packet);
	}
}
void Server::SendResetRoomInfo(char client)
{
	SC_PACKET_RESET_USERS_INFO packet;
	packet.size = sizeof(SC_PACKET_RESET_USERS_INFO);
	packet.type = SC_RESET_ROOM_INFO;
	SendFunc(client, &packet);
}

void Server::SendAcessComplete(char client)
{
	SC_PACKET_ACCESS_COMPLETE packet;
	packet.myId = client;
	packet.size = sizeof(packet);
	packet.type = SC_ACCESS_COMPLETE;
	// �ٵ� �α��� �Ǹ� �ٲٴ� �Ŷ� ȣ��Ʈ ����
	SendFunc(client, &packet);
}


void Server::SendAccessPlayer(char toClient, char fromClient)
{
	SC_PACKET_ACCESS_PLAYER packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_ACCESS_PLAYER;

	SendFunc(toClient, &packet);
}


void Server::SendGoLobby(char toClient)
{
	SC_PACKET_GO_LOBBY packet;

	packet.size = sizeof(packet);
	packet.type = SC_GO_LOBBY;

	SendFunc(toClient, &packet);
}

void Server::SendRoundEnd(char client)
{
	SC_PACKET_ROUND_END packet;
	packet.isWinner = true;
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (false == clients[i].in_use)
		{
			packet.checkPoints = 0;
		}
		else
			break;
	}
	packet.size = sizeof(SC_PACKET_ROUND_END);
	packet.type = SC_ROUND_END;

	SendFunc(client, &packet);
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

void Server::SendGameMode(char toClient)
{
	SC_PACKET_GAME_MODE_INFO packet;

	packet.size = sizeof(packet);
	packet.type = SC_GAME_MODE_INFO;
	packet.m_bGameMode = game_mode;


	SendFunc(toClient, &packet);
}


void Server::SendInGameReady(char client)
{
	SC_PACKET_INGAME_READY_INFO packet;

	packet.size = sizeof(packet);
	packet.type = SC_INGAME_READY;
	SendFunc(client, &packet);
}
void Server::SendInGameFinish(char client, string name)
{
	SC_PACKET_INGAME_FINISH_INFO packet;
	packet.size = sizeof(packet);
	packet.type = SC_INGAME_FINISH;
	packet.name = name;
	SendFunc(client, &packet);
}
void Server::SendPlayerInfo(char toClient, char fromClient)
{
	SC_PACKET_PLAYER_INFO packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_PLAYER_INFO;
	packet.xmf4x4Parents = clients[fromClient].xmf4x4Parents;
	packet.checkPoints = clients[fromClient].checkPoints;
	packet.keyState = clients[fromClient].keyState;
	packet.playerNames = clients[fromClient].game_id;

	/*char size;
	char type;
	char id;
	int checkPoints;
	DWORD keyState;
	XMFLOAT4X4 xmf4x4Parents;
	string playerNames;*/
	SendFunc(toClient, &packet);
}

//�÷��̾� Ư�� �ִϸ��̼ǿ� -> �̲����� �浹�ִϸ��̼�
void Server::SendPlayerAni_Collision(char toClient, char fromClient)
{
	SC_PACKET_PLAYER_ANI packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_COLLISION_ANI;
	SendFunc(toClient, &packet);
}
void Server::SendPlayerAni_Sliding(char toClient, char fromClient)
{
	SC_PACKET_PLAYER_ANI packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_SLIDING_ANI;
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


// �÷��̾� �ִϸ��̼� ó��
void Server::SendEventPacket(char toClient, MessageStruct& msg)
{
	SC_PACKET_EVENT packet;

	packet.id = toClient;
	packet.size = sizeof(packet);
	packet.type = SC_EVENT;
	packet.msg = msg;

	SendFunc(toClient, &packet);
}


//void Server::SendQuitClient(char toClient, char fromClient)
//{
//	SC_PACKET_REMOVE_PLAYER packet;
//	packet.id = fromClient;
//	packet.size = sizeof(packet);
//	packet.type = SC_REMOVE_PLAYER;
//
//	SendFunc(toClient, &packet);
//}

void Server::SendRemovePlayer(char fromClient)
{
	SC_PACKET_REMOVE_PLAYER packet;
	packet.id = fromClient;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_PLAYER;

	SendFunc(fromClient, &packet);
}

void Server::SendPutPlayer(char toClient)
{
	SC_PACKET_PUT_PLAYER packet;
	packet.size = sizeof(SC_PACKET_PUT_PLAYER);
	packet.type = SC_PUT_PLAYER;
	/*
	1. (650.0f, 70.7f, 1150.0f)
	2. (680.0f, 70.7f, 1150.0f)
	3. (710.0f, 70.7f, 1150.0f)
	4. (740.0f, 70.7f, 1150.0f)
	5. (770.0f, 70.7f, 1150.0f)
	*/
	XMFLOAT3 positions[5] = {XMFLOAT3(650.0f, 70.7f, 1150.0f),
	XMFLOAT3(680.0f, 70.7f, 1150.0f),
	XMFLOAT3(710.0f, 70.7f, 1150.0f),
	XMFLOAT3(740.0f, 70.7f, 1150.0f),
	XMFLOAT3(770.0f, 70.7f, 1150.0f)};
	packet.xmf3PutPos = positions[toClient];

	SendFunc(toClient, &packet);
}

void Server::ClientDisconnect(char client)
{
	clients[client].in_use = false; 
	SendRemovePlayer(client);
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (false == clients[i].in_use)
			continue;
		if (i == client)
			continue;
		//SendQuitClient(i, client);
		SendResetRoomInfo(i);
		SendRoomInfo(i);
	}
	closesocket(clients[client].socket);
	clientCnt_l.lock();
	--clientCount;
	printf("%d Ŭ���̾�Ʈ ���� ����, ���� Ŭ���̾�Ʈ ��: %d\n", (int)client, clientCount);
	clientCnt_l.unlock();
}

//void Server::ClientDisconnect(char client)
//{
//	clients[client].in_use = false;
//	clients[client].isReady = false;
//	SendRemovePlayer(client);
//	for (int i = 0; i < MAX_USER; ++i)
//	{
//		if (false == clients[i].in_use)
//			continue;
//		if (i == client)
//			continue;
//		SendResetRoomInfo(i);
//		SendRoomInfo(i);
//	}
//	closesocket(clients[client].socket);
//	clientCnt_l.lock();
//	--clientCount;
//	printf("%d Ŭ���̾�Ʈ ���� ����, ���� Ŭ���̾�Ʈ ��: %d\n", (int)client, clientCount);
//	clientCnt_l.unlock();
//}





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


// for DB
void Server::init_DB()
{
	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
			}
		}
	}

	std::cout << std::endl << "DB �ε忡 �����ϼ̽��ϴ�!" << std::endl;
}

int  Server::get_DB_Info(int ci)
{
	// Connect to data source  
	retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2015180014_Graduate", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

	// Allocate statement handle  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

		sprintf(buf, "EXEC dbo.Search_id %s", clients[ci].game_id);
		MultiByteToWideChar(CP_UTF8, 0, buf, strlen(buf), sql_data, sizeof sql_data / sizeof * sql_data);
		sql_data[strlen(buf)] = '\0';

		retcode = SQLExecDirect(hstmt, sql_data, SQL_NTS);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

			// Bind columns 1, 2, and 3  
			// �� DB�� �ִ� ������ id, ��й�ȣ(�̰� ���� �ʿ��ҷ���...?), ���ȣ, ���� or �𷹵� 
			retcode = SQLBindCol(hstmt, 1, SQL_WCHAR, sz_id, Default_LEN, &cb_id);
			retcode = SQLBindCol(hstmt, 2, SQL_INTEGER, &db_connect, Default_LEN, &cb_connect); 
			// �÷��̾� ��ü���� ������ �ɰ�����

			// Fetch and print each row of data. On an error, display a message and exit.  

			retcode = SQLFetch(hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				printf("ID : %s\n", sz_id);
				SQLDisconnect(hdbc);
				return DB_Success;
			}
		}

		if (retcode == SQL_NO_DATA) {
			SQLDisconnect(hdbc);
			return DB_NoData;
		}

		if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
			SQLDisconnect(hdbc);
			return DB_NoConnect;
		}
	}
	SQLDisconnect(hdbc);
}

void Server::set_DB_Info(int ci)
{
	// Connect to data source  
	retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2015180014_Graduate", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

	// Allocate statement handle  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

		sprintf(buf, "EXEC dbo.Set_id %s, %d\n", clients[ci].game_id, clients[ci].in_use );
		MultiByteToWideChar(CP_UTF8, 0, buf, strlen(buf), sql_data, sizeof sql_data / sizeof * sql_data);
		sql_data[strlen(buf)] = '\0';

		retcode = SQLExecDirect(hstmt, sql_data, SQL_NTS);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

			// Bind columns 1, 2, and 3  
			// �� DB�� �ִ� ������ id, ��й�ȣ(�̰� ���� �ʿ��ҷ���...?), ���ȣ, ���� or �𷹵� 
			retcode = SQLBindCol(hstmt, 1, SQL_WCHAR, sz_id, Default_LEN, &cb_id);
			retcode = SQLBindCol(hstmt, 2, SQL_INTEGER, &db_connect, Default_LEN, &cb_connect);

			// Fetch and print each row of data. On an error, display a message and exit.  

			retcode = SQLFetch(hstmt);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
				printf("set_DB_info error! \n");
			}

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				printf("ID : %s\n", sz_id);
			}

		}
	}
	SQLDisconnect(hdbc);
}

void Server::new_DB_Id(int ci)
{
	// Connect to data source  
	retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2015180014_Graduate", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
	printf("���⿡ ���� � ID�� �� ������?! %s\n", clients[ci].game_id);

	// Allocate statement handle  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

		sprintf(buf, "EXEC dbo.Set_id %s, %d\n", clients[ci].game_id, clients[ci].in_use);
		MultiByteToWideChar(CP_UTF8, 0, buf, strlen(buf), sql_data, sizeof sql_data / sizeof * sql_data);
		sql_data[strlen(buf)] = '\0';

		retcode = SQLExecDirect(hstmt, sql_data, SQL_NTS);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

			retcode = SQLFetch(hstmt);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
			}

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				printf("ID : %s\n", sz_id);
			}

		}
	}
	SQLDisconnect(hdbc);
}

void Server::set_DB_Shutdown(int ci)
{
	// Connect to data source  
	retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2015180014_Graduate", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

	// Allocate statement handle  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

		sprintf(buf, "EXEC dbo.shutdown_id %s", clients[ci].game_id);
		MultiByteToWideChar(CP_UTF8, 0, buf, strlen(buf), sql_data, sizeof sql_data / sizeof * sql_data);
		sql_data[strlen(buf)] = '\0';

		retcode = SQLExecDirect(hstmt, sql_data, SQL_NTS);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
			}

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				printf("ID : %s\n", sz_id);

			}

		}
	}
	SQLDisconnect(hdbc);
}
