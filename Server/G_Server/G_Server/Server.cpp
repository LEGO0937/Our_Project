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
	// 바로 DB설정
	Server::init_DB();

	clientCount = 0;
	// Winsock Start - windock.dll 로드
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		cout << "Error - Can not load 'winsock.dll' file\n";
		return false;
	}

	// 1. 소켓생성											Overlapped할거면 무조건 WSA_FLAG_OVERLAPPED
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		cout << "Error - Invalid socket\n";
		return false;
	}

	// 서버정보 객체설정
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	// 2. 소켓설정
	// std의 bind가 호출되므로 소켓의 bind를 불러주기 위해 앞에 ::붙임
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		cout << "Error - Fail bind\n";
		// 6. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return false;
	}

	// 3. 수신대기열생성
	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		cout << "Error - Fail listen\n";
		// 6. 소켓종료
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
		//행렬값 나중에 넣기
		
		//이부분에 클라한테 초기위치 요청하는 명령 보내고
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
		// clientSocket을 비동기식으로 만들기 위해서는 listenSocket이 비동기식이어야 한다.
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Error - Accept Failure\n";
			return;
		}
		// cout << "여기11" << endl;
		// id를 0~5까지 사용할 예정
		// 흠.. 이 아이디를 1부터 사용한다고...?
		int new_id = -1;
		for (int i = 0; i < MAX_USER; ++i)
		{
			// i를 인덱스로 하는 값이 몇 개가 있는가?
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
	

		//-----------------------DB 처리부분-----------------------------
		char  buf[255];
		int retval = recv(clientSocket, buf, 10, 0);
		if (retval == SOCKET_ERROR) {
			std::cout << "Not Recv Game_ID : " << new_id << std::endl;
			closesocket(clientSocket);
			continue;
		}
		buf[retval] = '\0';

		strcpy(clients[new_id].game_id, buf);
		printf_s("DB처리부의 Game ID: %s\n", clients[new_id].game_id);
		

		int is_accept = get_DB_Info(new_id);
		if (is_accept == DB_Success && db_connect == 0) {
			cout << "DB잘 받음\n" << endl;
			strcpy(buf, "Okay");
			retval = send(clientSocket, buf, strlen(buf), 0);
		}
		else if (is_accept == DB_NoConnect) {
			cout << "DB연결안됨\n" << endl;
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
			cout << "DB새로 만들자\n" << endl;
			//cout << new_id << endl;
			strcpy(buf, "Newid");

			new_DB_Id(new_id);
			get_DB_Info(new_id);
			retval = send(clientSocket, buf, strlen(buf), 0);
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		
		
		// 들어온 접속 아이디 init 처리

		///////////////////////////////////// 클라이언트 초기화 정보 수정 위치 /////////////////////////////////////
		clients[new_id].socket = clientSocket;

		SetClient_Initialize(new_id);
		///////////////////////////////////// 클라이언트 초기화 정보 수정 위치 /////////////////////////////////////

		

		flags = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), iocp, new_id, 0);

		clients[new_id].in_use = true;
		SendAcessComplete(new_id);
		SendGameMode(new_id);

		printf("%d 클라이언트 접속 완료\n", new_id);
		// 기존 유저들에게 이후 접속한 유저들 출력
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (true == clients[i].in_use)
			{
				SendAccessPlayer(i, new_id);
			}
		}


		// 처음 접속한 나에게 기존 유저들 출력
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (false == clients[i].in_use)
				continue;
			/*if (i == new_id)
				continue;*/

			SendAccessPlayer(new_id, i);
			SendResetRoomInfo(i);
			SendRoomInfo(i);
			// 여기다 방 목록 업데이트 추가로 넣자!
		}
		clientCnt_l.lock();
		++clientCount;
		printf("%d 클라이언트 접속 완료, 현재 클라이언트 수: %d\n", new_id, clientCount);
		clientCnt_l.unlock();
		RecvFunc(new_id);
	}
	// 6-2. 리슨 소켓종료
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
		// 비동기식으로 돌아가지 않고 동기식으로 돌아갔다는 오류.
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
		// 포인터의 포인터를 넘겨줘야
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
			// RECV 처리
			// cout << "Packet from Client: " << (int)key << "\n";
			// 패킷조립
			// 남은 크기
			int rest = io_byte;
			// 실제 버퍼
			char* ptr = over_ex->messageBuffer;
			char packet_size = 0;

			// 패킷 사이즈 알아내기 (중반부터 가능)
			if (0 < clients[key].prev_size)
				packet_size = clients[key].packet_buffer[0];

			while (0 < rest) {
				if (0 == packet_size) packet_size = ptr[0];	// ptr[0]이 지금부터 처리할 패킷
				// 패킷 처리하려면 얼마나 더 받아야 하는가?
				// 이전에 받은 조립되지 않은 패킷이 있을 수 있으니 prev_size 빼주기
				int required = packet_size - clients[key].prev_size;
				if (required <= rest) {
					// 패킷 만들 수 있는 경우

					// 앞에 와있던 데이터가 저장되어있을 수 있으니 그 뒤에 저장되도록 prev_size를 더해준다.
					memcpy(clients[key].packet_buffer + clients[key].prev_size, ptr, required);
					ProcessPacket(key, clients[key].packet_buffer);
					rest -= required;
					ptr += required;
					// 이미 계산됐기 때문에 다음 패킷을 처리할 수 있도록 0
					packet_size = 0;
					clients[key].prev_size = 0;
				}
				else {
					// 패킷 만들 수 없는 경우
					memcpy(clients[key].packet_buffer + clients[key].prev_size, ptr, rest);
					rest = 0;
					clients[key].prev_size += rest;
				}
			}

			RecvFunc(key);
		}
		else if(EV_SEND == over_ex->event_t)
		{
			// SEND 처리
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
	// 0번은 사이즈, 1번이 패킷타입
	// packet[0] packet[1] 

	DWORD tmpDir;
	bool isInGameReady = false;
	switch (packet[1]) // 0번째는 패킷 사이즈 1번쨰는 패킷 타입
	{
	case CS_PLAYER_INFO: // 플레이어 전체적인 위치 담당
	{
		//printf("Player Info Success: %d\n", client);
		// 각 클라에게 위치 인덱스 전송
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
		// 클라가 엔터누르고 F5누를때마다 CS_READY 패킷이 날아온다면 ++readyCount는 clientCount보다 증가하게 되고 
		// 아래 CS_REQUEST_START안에 if(clientCount<= readyCount) 안으로 들어가지 않는 현상 발생
		readyCnt_l.lock();
		clientCnt_l.lock();
		printf("전체 클라 수: %d\n", clientCount);
		if (readyCount < clientCount - 1)
			++readyCount;
		printf("Ready한 클라 수: %d\n", readyCount);
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
			// 여기다 방 목록 업데이트 추가로 넣자!
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
		printf("Ready한 클라 수: %d\n", readyCount);
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
			// 여기다 방 목록 업데이트 추가로 넣자!
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


			// 이 주석부분에 초기 위치 설정

		/*	for (int i = 0; i < MAX_USER; ++i)
			{
				if (clients[i].in_use == true)
				{
					clients[i].gameState = GS_INGAME;
				}
			}*/


			// 랜덤으로 결정된 시작위치인덱스에 해당하는 위치 적용
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (false == clients[i].in_use)
					continue;
				
			}

			// 각 클라에게 시작위치 인덱스 전송
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
			////이 부분 READYCOUNT 보다
			//for (int i = 0; i < MAX_USER; ++i)
			//{
			//	if (true == clients[i].in_use)
			//		SendPleaseReady(i);
			//}
			// 나중에 채팅 서버 구현되면 적용
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
		wcout << L"패킷 사이즈: " << (int)packet[0] << endl;
		wcout << L"패킷 타입 : " << (int)packet[1] << endl;
		cout << "Client prev size : " << clients[client].prev_size << "\n";
		for (int i = 2; i < (int)packet[0]; ++i)
			wcout << packet[i];
		wcout << L"\n";
		wcout << L"정의되지 않은 패킷 도착 오류!!\n";
		break;
	}
}



void Server::SendFunc(char client, void* packet)
{
	char* p = reinterpret_cast<char*>(packet);
	OVER_EX* ov = new OVER_EX;
	ov->dataBuffer.len = p[0];
	ov->dataBuffer.buf = ov->messageBuffer;
	//ov->is_recv = false; // 이벤트 타입으로 수정ㅇㄴㄹㄴㅁ
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
		// 비동기식으로 돌아가지 않고 동기식으로 돌아갔다는 오류.
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
	// 다들 로그인 되면 바꾸는 거라 호스트 삭제
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

//플레이어 특수 애니메이션용 -> 미끄러짐 충돌애니메이션
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

// 플레이어 인포로 사용...? 
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


// 플레이어 애니메이션 처리
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
	printf("%d 클라이언트 접속 종료, 현재 클라이언트 수: %d\n", (int)client, clientCount);
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
//	printf("%d 클라이언트 접속 종료, 현재 클라이언트 수: %d\n", (int)client, clientCount);
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

	std::cout << std::endl << "DB 로드에 성공하셨습니다!" << std::endl;
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
			// 이 DB에 있는 정보는 id, 비밀번호(이건 굳이 필요할려나...?), 방번호, 레디 or 언레디 
			retcode = SQLBindCol(hstmt, 1, SQL_WCHAR, sz_id, Default_LEN, &cb_id);
			retcode = SQLBindCol(hstmt, 2, SQL_INTEGER, &db_connect, Default_LEN, &cb_connect); 
			// 플레이어 객체에다 넣으면 될것이지

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
			// 이 DB에 있는 정보는 id, 비밀번호(이건 굳이 필요할려나...?), 방번호, 레디 or 언레디 
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
	printf("여기에 과연 어떤 ID가 들어가 있을라나?! %s\n", clients[ci].game_id);

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
