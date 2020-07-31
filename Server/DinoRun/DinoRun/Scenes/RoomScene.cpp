#include "RoomScene.h"

#include "../Common/FrameWork/CreateManager.h"
#include "../Common/FrameWork/NetWorkManager.h"
#include "../Common/FrameWork/SoundManager.h"

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
	SoundManager::GetInstance()->Stop("Start_BGM");
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
	//네트워크 클래스에 저장되어있는 방번호를 보내고 해당 방에서의 플레이어 정보를 받아온다.

	//*서버*
	//들어오기 성공을 하면 방번호와 자신의 닉네임을 서버에게 보냄 
	//서버는 해당 번호의 방 정보에 유저를 추가한다.
	//서버는 클라 자신의 정보를 제외한 나머지 유저들의 이름과 버튼 상태를 전송한다.-> User클래스 사용


	//반드시 게임을 시작할 땐 나 자신과 최소 한명 이상의 타 플레이어가 있어야만 함.

	uiShader = new BackGroundShader;
	string name = "Resources/Images/T_Room.dds";
	uiShader->BuildObjects(pCreateManager.get(), &name);
	instacingUiShaders.emplace_back(uiShader);

	//이 부분에서 먼저 타플레이어의 정보(이름,버튼상태)를 받아온다. 
	m_vUsers.emplace_back(User("user1", true));
	//m_vUsers.emplace_back(User("user2", true));
	//m_vUsers.emplace_back(User("user3", true));
	//m_vUsers.emplace_back(User("user4", true));

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


	instacingUiShaders[1]->getUvXs()[0] = 0.5f;
	CreateShaderVariables(pCreateManager.get());
}

void RoomScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	Point2D point;
	::SetCapture(hWnd);
	::GetCursorPos(&m_ptOldCursorPos);
	ScreenToClient(hWnd, &m_ptOldCursorPos);
	point = ScreenToProj(m_nWndClientWidth, m_nWndClientHeight, m_ptOldCursorPos);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		SoundManager::GetInstance()->Play("Mouse_Down", 0.2f);
		if (point.x > -0.15f && point.x < 0.15f && point.y > -0.92f && point.y < -0.68f) //로그인 버튼 충돌체크
		{
			if (instacingUiShaders[1]->getUvXs()[0] == 0.0f)
			{
				BUTTON_STATE_SHADER->getUvXs()[0] = 0.5f;
				BUTTON_STATE_SHADER->getUvXs()[1] = 0.0f;
				//임시적인 씬이동을 위해 여기서 씬타입 전환
				//sceneType = ItemGame_Scene;
				//클릭마다 네트워크에 버튼상태 전송할것 send,recv
			}
			else
			{
				BUTTON_STATE_SHADER->getUvXs()[0] = 0.0f;
				BUTTON_STATE_SHADER->getUvXs()[1] = 0.5f;
			}
			//*서버*
			//버튼이 클릭 되는 구간
			//클릭하여 버튼상태 변화 시 클라에게 버튼의 현재상태 정보를 보냄
			//판정에 따라 한번 클릭되면 레디, 두번 누르면 레디 안되게 설정
			//NetWorkManager::GetInstance()->SendReady();
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
			sceneType = SceneType::Lobby_Scene;
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

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList, m_pCamera, gameTexts);

#ifdef _WITH_BOUND_BOX

#endif
}


void RoomScene::AnimateObjects(float fTimeElapsed)
{
	//말 그대로 애니메이션 update
}


SceneType RoomScene::Update(CreateManager* pCreateManager, float fTimeElapsed)
{
	if (sceneType != SceneType::Room_Scene)
	{
		return sceneType;
	}
	//*서버*
	//먼저 방에 있는 인원이 full이고 모두 레디한 상태인지 물어볼 것이다.
	//준비가 모두 되어 있다면 게임씬으로 이동.
	//안돼있다면 vector를 clear하고 
	//본인의 닉네임을 보내주고나서 다시 유저들의 이름 및 버튼 상태의 정보를 받아서 vector<User>에 담는다.
	//이 때 클라 본인의 정보는 받지 않아야 함.
	//m_vUsers.clear();
	//m_vUsers.emplace_back(User("user1", true));
	//m_vUsers.emplace_back(User("user2", true));
	//m_vUsers.emplace_back(User("user3", true));
	//m_vUsers.emplace_back(User("user4", true));
	//clear로 m_vUsers초기화 후 send,recv로 현재 방안에 있는 유저의 정보를 받아와서 최신화한다.
	//send때 자신의 방번호와 닉네임을 보내고 서버에서는 이 닉네임을 제외한 다른 유저들의 정보를
	//넘긴다.
	for (int i = 0; i < m_vUsers.size(); ++i)
	{
		if (m_vUsers[i].m_sName == m_sPlayerId)
			continue;
		if (i < m_vUsers.size())
		{
			gameTexts[i + 1].text = m_vUsers[i].m_sName;  //첫번째는 본인이름이 들어가니 두번째란부터 입력
			instacingUiShaders[1]->getUvXs()[i + 2] = 0.5 * m_vUsers[i].m_bButtonState;
		}
		else
		{
			gameTexts[i + 1].text = "";
			instacingUiShaders[1]->getUvXs()[i + 2] = 0.5;
		}
	}
	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);

	//아래 반복문은 삭제하고 send,Recv로 방번호를 주고 현 방안에 있는 유저들이 모두 레디했는지를
	//확인한다. 다 됐을 시 네트워크매니저에 저장돼있는 게임모드로 씬전환.

	//선택1. 서버에서 모든 유저의  상태를 갖고있을테니 거기서 유저수가 2이상이고 
	//방에 있는 유저들이 모두 ready상태라면 클라에게 시작하라는 신호를 주는 방법
	//선택2. 위의 ready상태 확인을 클라에서하는 방법

	for (int i = 0; i < m_vUsers.size(); ++i) //이 부분은 user의 수가 최소 1이상일 경우에만 체크하도록 한다.
	{
		if (instacingUiShaders[1]->getUvXs()[i + 1] != 0.5f && m_vUsers[i].m_sName != "")
			return SceneType::Room_Scene;
	}
	//네트워크 클래스에 저장되있는 방 모드 종류에 따라서 다른 게임씬전환
	if (m_vUsers.size() == 0)
		return SceneType::Room_Scene;
	NetWorkManager::GetInstance()->SetNumPlayer(m_vUsers.size());
	if (NetWorkManager::GetInstance()->GetGameMode())
		sceneType = SceneType::ItemGame_Scene;
	else
		sceneType = SceneType::Game_Scene;

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
	case SC_READY_STATE:
		UpdateReadyState(packet, fTimeElapsed);
		break;
	case SC_UNREADY_STATE:
		UpdateUnreadyState(packet, fTimeElapsed);
		break;
	case SC_CLIENT_LOBBY_IN:
		UpdateAddUser(packet, fTimeElapsed);
		break;
	case SC_CLIENT_LOBBY_OUT:
		UpdateDeleteUser(packet, fTimeElapsed);
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
	//m_vUsers
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

