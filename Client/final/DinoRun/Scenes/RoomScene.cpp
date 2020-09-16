#include "RoomScene.h"

#include "../Common/FrameWork/CreateManager.h"
#include "../Common/FrameWork/NetWorkManager.h"
#include "../Common/FrameWork/SoundManager.h"
#include "EventHandler/EventHandler.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"

#define BUTTON_STATE_SHADER instacingUiShaders[1]

RoomScene::RoomScene() :BaseScene()
{
	sceneType = SceneType::Room_Scene;
}
RoomScene::~RoomScene()
{

}
void RoomScene::ReleaseUploadBuffers()
{
	BaseScene::ReleaseUploadBuffers();

	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
}
void RoomScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();

	m_vUsers.clear();
	
	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }

}
void RoomScene::BuildObjects(shared_ptr<CreateManager> pCreateManager)
{
	m_pCreateManager = pCreateManager;

	m_pd3dCommandList = pCreateManager->GetCommandList().Get();


	SoundManager::GetInstance()->Play("Start_BGM", 0.2f);
	CUiShader* uiShader;

	//반드시 게임을 시작할 땐 나 자신과 최소 한명 이상의 타 플레이어가 있어야만 함.

	uiShader = new BackGroundShader;
	string name = "Resources/Images/T_Room.dds";
	uiShader->BuildObjects(pCreateManager.get(), &name);
	instacingUiShaders.emplace_back(uiShader);

#ifdef isConnectedToServer
#else
	m_vUsers.emplace_back(User("user1", true));
#endif


	UI_INFO button_info;
	button_info.textureName = "Resources/Images/T_Button.dds";
	button_info.meshSize = XMFLOAT2(0.15f, 0.12f);
	button_info.positions.emplace_back(XMFLOAT3(0.0f, -0.8f, 0.0f));
	button_info.f_uvY.emplace_back(0.0f);
	//레디 or 준비 상태 확인 인터페이스
	button_info.positions.emplace_back(XMFLOAT3(0.55f, 0.57f, 0.0f));
	button_info.f_uvY.emplace_back(0.0f);
	button_info.positions.emplace_back(XMFLOAT3(0.55f, 0.28f, 0.0f));
	button_info.f_uvY.emplace_back(0.0f);
	button_info.positions.emplace_back(XMFLOAT3(0.55f, 0.0f, 0.0f));
	button_info.f_uvY.emplace_back(0.0f);
	button_info.positions.emplace_back(XMFLOAT3(0.55f, -0.28f, 0.0f));
	button_info.f_uvY.emplace_back(0.0f);
	button_info.positions.emplace_back(XMFLOAT3(0.55f, -0.57f, 0.0f));
	button_info.f_uvY.emplace_back(0.0f);
	//---------------------------스피드전 아이템전 선택 버튼
	button_info.positions.emplace_back(XMFLOAT3(-0.5f, -0.8f, 0.0f));
	button_info.f_uvY.emplace_back(0.75f);

	button_info.maxUv = XMFLOAT2(0.5f, 0.25f);
	button_info.minUv = XMFLOAT2(0.0f, 0.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &button_info);
	instacingUiShaders.emplace_back(uiShader);


	gameTexts.emplace_back(GameText(XMFLOAT2(0.17f, 0.19f))); //플레이어 명단
	gameTexts.emplace_back(GameText(XMFLOAT2(0.17f, 0.33f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.17f, 0.47f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.17f, 0.61f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.17f, 0.75f)));
	gameTexts[0].text = m_sPlayerId;


	BUTTON_STATE_SHADER->getUvXs()[0] = 0.5f;
	CreateShaderVariables(pCreateManager.get());

#ifdef isConnectedToServer
	NetWorkManager::GetInstance()->SendNotReady();
#endif
}

void RoomScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	Point2D point;
	::SetCapture(hWnd);
	::GetCursorPos(&m_ptOldCursorPos);
	ScreenToClient(hWnd, &m_ptOldCursorPos);
	point = ScreenToProj(EventHandler::GetInstance()->m_nWndClientWidth, EventHandler::GetInstance()->m_nWndClientHeight, m_ptOldCursorPos);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		SoundManager::GetInstance()->Play("Mouse_Down", 0.2f);
		if (point.x > -0.15f && point.x < 0.15f && point.y > -0.92f && point.y < -0.68f) //레디 버튼 충돌체크
		{
			if (instacingUiShaders[1]->getUvXs()[0] == 0.0f)
			{
				//레디-> 취소 상태로 버튼 전환하는 구간
				//수행 후 버튼은 레디상태가 됨
#ifdef isConnectedToServer
				NetWorkManager::GetInstance()->SendNotReady();
#endif
				BUTTON_STATE_SHADER->getUvXs()[0] = 0.5f;
				BUTTON_STATE_SHADER->getUvXs()[1] = 0.0f;
			}
			else
			{
#ifdef isConnectedToServer
				NetWorkManager::GetInstance()->SendReady();
#endif
				//취소-> 레디 상태로 버튼 전환하는 구간
				BUTTON_STATE_SHADER->getUvXs()[0] = 0.0f;
				BUTTON_STATE_SHADER->getUvXs()[1] = 0.5f;
			}

		}
		else if (point.x > -0.65f && point.x < -0.35f && point.y > -0.92f && point.y < -0.68f) //모드선택버튼 충돌체크
		{
			{
				
#ifdef isConnectedToServer
				NetWorkManager::GetInstance()->SendChangeGameMode();
				//모드 상태 바뀌었다는 패킷전송
#else
				m_bModeState = !m_bModeState;
#endif

			}
		}
		::ReleaseCapture();
		break;
	case WM_RBUTTONDOWN:
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다. 
		break;
	case WM_LBUTTONUP:
		SoundManager::GetInstance()->Play("Mouse_Up", 0.2f);
		::ReleaseCapture();
		break;
	case WM_RBUTTONUP:
		//마우스 캡쳐를 해제한다. 

		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}
void RoomScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
#ifdef isConnectedToServer
			//서버에게 나간다는 메시지 send
			NetWorkManager::GetInstance()->SendRemovePlayer();
#else
			sceneType = SceneType::Start_Scene;
#endif
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_BACK:			
			break;

		default:
			break;
		}
		break;
	default:
		break;
	}
}
void RoomScene::ProcessInput(HWND hwnd, float deltaTime)
{
}

void RoomScene::Render()
{
	BaseScene::Render();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_BILLBOARD]);
	
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	if (instacingUiShaders[0])
		instacingUiShaders[0]->Render(m_pd3dCommandList, m_pCamera);
	if (instacingUiShaders[1])
		instacingUiShaders[1]->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_FONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList, m_pCamera, gameTexts);

#ifdef _WITH_BOUND_BOX

#endif
}


void RoomScene::AnimateObjects(float fTimeElapsed)
{

}

SceneType RoomScene::Update(CreateManager* pCreateManager, float fTimeElapsed)
{
	if (sceneType != SceneType::Room_Scene)
	{
		SoundManager::GetInstance()->AllStop();
		return sceneType;
	}
	
	BUTTON_STATE_SHADER->getUvXs()[6] = 0.5f * m_bModeState;

	for (int i = 0; i < 4; ++i)
	{

		if (i < m_vUsers.size())
		{
			gameTexts[i + 1].text = m_vUsers[i].m_sName;  //첫번째는 본인이름이 들어가니 두번째란부터 입력
			BUTTON_STATE_SHADER->getUvXs()[i + 2] = 0.5f * (float)m_vUsers[i].m_bButtonState;
		}
		else
		{
			gameTexts[i + 1].text = "";
			BUTTON_STATE_SHADER->getUvXs()[i + 2] = 0.5f;
		}
	}
	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);
	if (BUTTON_STATE_SHADER->getUvXs()[1] != 0.5f)
		return SceneType::Room_Scene;

	for (int i = 0; i < m_vUsers.size(); ++i)
	{
		//이 부분은 들어와있는 모든 유저가 레디상태인지 확인 한명이라도 취소상태이면 다음씬으로 안넘어감.
		if (instacingUiShaders[1]->getUvXs()[i + 2] != 0.5f && m_vUsers[i].m_sName != "")
			return SceneType::Room_Scene; 
	}
	//이 구간에 왔다는것은 접속한 유저들이 모두 레디상태라는 뜻.
	//마지막으로 나를 제외한 유저가 한명이라도 있다면 두명 이상 레디상태이니 다음 게임씬으로 넘어간다.

	//네트워크 클래스에 저장되있는 방 모드 종류에 따라서 다른 게임씬전환
	if (m_vUsers.size() == 0)
		return SceneType::Room_Scene;
	
#ifdef isConnectedToServer
	NetWorkManager::GetInstance()->SendReqStart();
#else
	NetWorkManager::GetInstance()->SetNumPlayer(m_vUsers.size());  //게임을 할 유저 수(자신 제외) 공룡객체 만드는 수와 일치
	
	if (m_bModeState)
		sceneType = SceneType::ItemGame_Scene;
	else
		sceneType = SceneType::Game_Scene;
#endif
	// 스피드전 or 아이템전 판단.

	return SceneType::Room_Scene;
}


void RoomScene::CreateShaderVariables(CreateManager* pCreateManager)
{
}


void RoomScene::UpdateShaderVariables()
{
}

void RoomScene::ReleaseShaderVariables()
{

}


void RoomScene::setPlayer(CPlayer* player)
{
	BaseScene::setPlayer(player);
}

void RoomScene::setCamera(CCamera* camera)
{
	BaseScene::setCamera(camera);
}

void RoomScene::ProcessPacket(char* packet, float fTimeElapsed)
{
	switch (packet[1])
	{
	case SC_ACCESS_COMPLETE:
		UpdateAccessUser(packet, fTimeElapsed);
		break;
	case SC_PUT_PLAYER:   //모든 플레이어가 레디를 하였으니 게임모드로 넘어가라는 명령
		UpdateNextScene(packet, fTimeElapsed);
		break;
	case SC_ROOM_INFO:
		UpdateUserList(packet, fTimeElapsed);
		break; // 플레이어 리스트 및 레디 상태
	case SC_RESET_ROOM_INFO:
		UpdateClearUserList(packet, fTimeElapsed);
		break;
	case SC_GAME_MODE_INFO:
		UpdateModeState(packet, fTimeElapsed);
		break;
	case SC_REMOVE_PLAYER:
		sceneType = SceneType::Start_Scene;

		break;
	}
}
void RoomScene::UpdateUnreadyState(char* packet, float fTimeElapsed)
{
	SC_PACKET_UNREADY_STATE* playerInfo = reinterpret_cast<SC_PACKET_UNREADY_STATE*>(packet);
	auto obj = find_if(m_vUsers.begin(), m_vUsers.end(), [&](const User& a) {
		return a.m_id == playerInfo->id; });
	if (obj != m_vUsers.end())
	{
		(*obj).m_bButtonState = false;
	}
}

void RoomScene::UpdateReadyState(char* packet, float fTimeElapsed)
{
	SC_PACKET_READY_STATE* playerInfo = reinterpret_cast<SC_PACKET_READY_STATE*>(packet);
	auto obj = find_if(m_vUsers.begin(), m_vUsers.end(), [&](const User& a) {
		return a.m_id == playerInfo->id; });
	if (obj != m_vUsers.end())
	{
		(*obj).m_bButtonState = true;
	}
}

void RoomScene::UpdateAddUser(char* packet, float fTimeElapsed)
{
	//패킷 구조체안에 현재 버튼이 어떤 상태인지도 보내줄 필요가 있음. 
	SC_PACKET_LOBBY_IN* playerInfo = reinterpret_cast<SC_PACKET_LOBBY_IN*>(packet);

	auto obj = find_if(m_vUsers.begin(), m_vUsers.end(), [&](const User& a) {
		return a.m_id == playerInfo->id; });
	if (obj == m_vUsers.end())
	{
		User user;
		user.m_sName = playerInfo->client_state.name;
		user.m_bButtonState = false;
		user.m_id = playerInfo->id; 
		m_vUsers.emplace_back(user);
	}
}
void RoomScene::UpdateDeleteUser(char* packet, float fTimeElapsed)
{
	SC_PACKET_LOBBY_OUT* playerInfo = reinterpret_cast<SC_PACKET_LOBBY_OUT*>(packet);

	m_vUsers.erase(remove_if(m_vUsers.begin(), m_vUsers.end(), [&](const User& a) {
		return a.m_id == playerInfo->id; }), m_vUsers.end());
}

void RoomScene::UpdateLogOut(char* packet, float fTimeElapsed)
{
#ifdef noLobby
	sceneType = SceneType::Start_Scene;
#else
	sceneType = SceneType::Lobby_Scene;
#endif
}
void RoomScene::UpdateUserList(char* packet, float fTimeElapsed)
{
	SC_PACKET_USERS_INFO* usersInfo = reinterpret_cast<SC_PACKET_USERS_INFO*>(packet);

	{
		if (usersInfo->users.m_sName != "" && 
			usersInfo->users.m_sName != NetWorkManager::GetInstance()->GetPlayerName())
		{
			m_vUsers.emplace_back(usersInfo->users.m_sName, usersInfo->users.m_bReadyState);
		}
	}
}
void RoomScene::UpdateNextScene(char* packet, float fTimeElapsed)
{
	NetWorkManager::GetInstance()->SetNumPlayer(m_vUsers.size());  //게임을 할 유저 수(자신 제외) 공룡객체 만드는 수와 일치

	SC_PACKET_PUT_PLAYER* playerInfo = reinterpret_cast<SC_PACKET_PUT_PLAYER*>(packet);
	NetWorkManager::GetInstance()->SetPosition(playerInfo->xmf3PutPos);

	if (m_bModeState)
		sceneType = SceneType::ItemGame_Scene;
	else
		sceneType = SceneType::Game_Scene;

}

void RoomScene::UpdateAccessUser(char* packet, float fTimeElapsed)
{
	SC_PACKET_ACCESS_COMPLETE* access = reinterpret_cast<SC_PACKET_ACCESS_COMPLETE*>(packet);
	NetWorkManager::GetInstance()->SetMyID(access->myId);
	NetWorkManager::GetInstance()->SetPlayerName(m_sPlayerId);

}

void RoomScene::UpdateClearUserList(char* packet, float fTimeElapsed)
{
	m_vUsers.clear();
}

void RoomScene::UpdateModeState(char* packet, float fTimeElapsed)
{
	SC_PACKET_GAME_MODE_INFO* gameMode = reinterpret_cast<SC_PACKET_GAME_MODE_INFO*>(packet);
	m_bModeState = gameMode->m_bGameMode;
}