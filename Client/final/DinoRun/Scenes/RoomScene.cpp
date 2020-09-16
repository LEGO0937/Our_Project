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

	//�ݵ�� ������ ������ �� �� �ڽŰ� �ּ� �Ѹ� �̻��� Ÿ �÷��̾ �־�߸� ��.

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
	//���� or �غ� ���� Ȯ�� �������̽�
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
	//---------------------------���ǵ��� �������� ���� ��ư
	button_info.positions.emplace_back(XMFLOAT3(-0.5f, -0.8f, 0.0f));
	button_info.f_uvY.emplace_back(0.75f);

	button_info.maxUv = XMFLOAT2(0.5f, 0.25f);
	button_info.minUv = XMFLOAT2(0.0f, 0.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &button_info);
	instacingUiShaders.emplace_back(uiShader);


	gameTexts.emplace_back(GameText(XMFLOAT2(0.17f, 0.19f))); //�÷��̾� ���
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
		if (point.x > -0.15f && point.x < 0.15f && point.y > -0.92f && point.y < -0.68f) //���� ��ư �浹üũ
		{
			if (instacingUiShaders[1]->getUvXs()[0] == 0.0f)
			{
				//����-> ��� ���·� ��ư ��ȯ�ϴ� ����
				//���� �� ��ư�� ������°� ��
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
				//���-> ���� ���·� ��ư ��ȯ�ϴ� ����
				BUTTON_STATE_SHADER->getUvXs()[0] = 0.0f;
				BUTTON_STATE_SHADER->getUvXs()[1] = 0.5f;
			}

		}
		else if (point.x > -0.65f && point.x < -0.35f && point.y > -0.92f && point.y < -0.68f) //��弱�ù�ư �浹üũ
		{
			{
				
#ifdef isConnectedToServer
				NetWorkManager::GetInstance()->SendChangeGameMode();
				//��� ���� �ٲ���ٴ� ��Ŷ����
#else
				m_bModeState = !m_bModeState;
#endif

			}
		}
		::ReleaseCapture();
		break;
	case WM_RBUTTONDOWN:
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�. 
		break;
	case WM_LBUTTONUP:
		SoundManager::GetInstance()->Play("Mouse_Up", 0.2f);
		::ReleaseCapture();
		break;
	case WM_RBUTTONUP:
		//���콺 ĸ�ĸ� �����Ѵ�. 

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
			//�������� �����ٴ� �޽��� send
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
			gameTexts[i + 1].text = m_vUsers[i].m_sName;  //ù��°�� �����̸��� ���� �ι�°������ �Է�
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
		//�� �κ��� �����ִ� ��� ������ ����������� Ȯ�� �Ѹ��̶� ��һ����̸� ���������� �ȳѾ.
		if (instacingUiShaders[1]->getUvXs()[i + 2] != 0.5f && m_vUsers[i].m_sName != "")
			return SceneType::Room_Scene; 
	}
	//�� ������ �Դٴ°��� ������ �������� ��� ������¶�� ��.
	//���������� ���� ������ ������ �Ѹ��̶� �ִٸ� �θ� �̻� ��������̴� ���� ���Ӿ����� �Ѿ��.

	//��Ʈ��ũ Ŭ������ ������ִ� �� ��� ������ ���� �ٸ� ���Ӿ���ȯ
	if (m_vUsers.size() == 0)
		return SceneType::Room_Scene;
	
#ifdef isConnectedToServer
	NetWorkManager::GetInstance()->SendReqStart();
#else
	NetWorkManager::GetInstance()->SetNumPlayer(m_vUsers.size());  //������ �� ���� ��(�ڽ� ����) ���水ü ����� ���� ��ġ
	
	if (m_bModeState)
		sceneType = SceneType::ItemGame_Scene;
	else
		sceneType = SceneType::Game_Scene;
#endif
	// ���ǵ��� or �������� �Ǵ�.

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
	case SC_PUT_PLAYER:   //��� �÷��̾ ���� �Ͽ����� ���Ӹ��� �Ѿ��� ���
		UpdateNextScene(packet, fTimeElapsed);
		break;
	case SC_ROOM_INFO:
		UpdateUserList(packet, fTimeElapsed);
		break; // �÷��̾� ����Ʈ �� ���� ����
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
	//��Ŷ ����ü�ȿ� ���� ��ư�� � ���������� ������ �ʿ䰡 ����. 
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
	NetWorkManager::GetInstance()->SetNumPlayer(m_vUsers.size());  //������ �� ���� ��(�ڽ� ����) ���水ü ����� ���� ��ġ

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