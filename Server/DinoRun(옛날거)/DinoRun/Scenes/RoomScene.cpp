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
	//��Ʈ��ũ Ŭ������ ����Ǿ��ִ� ���ȣ�� ������ �ش� �濡���� �÷��̾� ������ �޾ƿ´�.

	//*����*
	//������ ������ �ϸ� ���ȣ�� �ڽ��� �г����� �������� ���� 
	//������ �ش� ��ȣ�� �� ������ ������ �߰��Ѵ�.
	//������ Ŭ�� �ڽ��� ������ ������ ������ �������� �̸��� ��ư ���¸� �����Ѵ�.-> UserŬ���� ���


	//�ݵ�� ������ ������ �� �� �ڽŰ� �ּ� �Ѹ� �̻��� Ÿ �÷��̾ �־�߸� ��.

	uiShader = new BackGroundShader;
	string name = "Resources/Images/T_Room.dds";
	uiShader->BuildObjects(pCreateManager.get(), &name);
	instacingUiShaders.emplace_back(uiShader);

	//�� �κп��� ���� Ÿ�÷��̾��� ����(�̸�,��ư����)�� �޾ƿ´�. 
	m_vUsers.emplace_back(User("user1", true));
	//m_vUsers.emplace_back(User("user2", true));
	//m_vUsers.emplace_back(User("user3", true));
	//m_vUsers.emplace_back(User("user4", true));

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
		if (point.x > -0.15f && point.x < 0.15f && point.y > -0.92f && point.y < -0.68f) //�α��� ��ư �浹üũ
		{
			if (instacingUiShaders[1]->getUvXs()[0] == 0.0f)
			{
				BUTTON_STATE_SHADER->getUvXs()[0] = 0.5f;
				BUTTON_STATE_SHADER->getUvXs()[1] = 0.0f;
				//�ӽ����� ���̵��� ���� ���⼭ ��Ÿ�� ��ȯ
				//sceneType = ItemGame_Scene;
				//Ŭ������ ��Ʈ��ũ�� ��ư���� �����Ұ� send,recv
			}
			else
			{
				BUTTON_STATE_SHADER->getUvXs()[0] = 0.0f;
				BUTTON_STATE_SHADER->getUvXs()[1] = 0.5f;
			}
			//*����*
			//��ư�� Ŭ�� �Ǵ� ����
			//Ŭ���Ͽ� ��ư���� ��ȭ �� Ŭ�󿡰� ��ư�� ������� ������ ����
			//������ ���� �ѹ� Ŭ���Ǹ� ����, �ι� ������ ���� �ȵǰ� ����
			//NetWorkManager::GetInstance()->SendReady();
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
	//�� �״�� �ִϸ��̼� update
}


SceneType RoomScene::Update(CreateManager* pCreateManager, float fTimeElapsed)
{
	if (sceneType != SceneType::Room_Scene)
	{
		return sceneType;
	}
	//*����*
	//���� �濡 �ִ� �ο��� full�̰� ��� ������ �������� ��� ���̴�.
	//�غ� ��� �Ǿ� �ִٸ� ���Ӿ����� �̵�.
	//�ȵ��ִٸ� vector�� clear�ϰ� 
	//������ �г����� �����ְ��� �ٽ� �������� �̸� �� ��ư ������ ������ �޾Ƽ� vector<User>�� ��´�.
	//�� �� Ŭ�� ������ ������ ���� �ʾƾ� ��.
	//m_vUsers.clear();
	//m_vUsers.emplace_back(User("user1", true));
	//m_vUsers.emplace_back(User("user2", true));
	//m_vUsers.emplace_back(User("user3", true));
	//m_vUsers.emplace_back(User("user4", true));
	//clear�� m_vUsers�ʱ�ȭ �� send,recv�� ���� ��ȿ� �ִ� ������ ������ �޾ƿͼ� �ֽ�ȭ�Ѵ�.
	//send�� �ڽ��� ���ȣ�� �г����� ������ ���������� �� �г����� ������ �ٸ� �������� ������
	//�ѱ��.
	for (int i = 0; i < m_vUsers.size(); ++i)
	{
		if (m_vUsers[i].m_sName == m_sPlayerId)
			continue;
		if (i < m_vUsers.size())
		{
			gameTexts[i + 1].text = m_vUsers[i].m_sName;  //ù��°�� �����̸��� ���� �ι�°������ �Է�
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

	//�Ʒ� �ݺ����� �����ϰ� send,Recv�� ���ȣ�� �ְ� �� ��ȿ� �ִ� �������� ��� �����ߴ�����
	//Ȯ���Ѵ�. �� ���� �� ��Ʈ��ũ�Ŵ����� ������ִ� ���Ӹ��� ����ȯ.

	//����1. �������� ��� ������  ���¸� ���������״� �ű⼭ �������� 2�̻��̰� 
	//�濡 �ִ� �������� ��� ready���¶�� Ŭ�󿡰� �����϶�� ��ȣ�� �ִ� ���
	//����2. ���� ready���� Ȯ���� Ŭ�󿡼��ϴ� ���

	for (int i = 0; i < m_vUsers.size(); ++i) //�� �κ��� user�� ���� �ּ� 1�̻��� ��쿡�� üũ�ϵ��� �Ѵ�.
	{
		if (instacingUiShaders[1]->getUvXs()[i + 1] != 0.5f && m_vUsers[i].m_sName != "")
			return SceneType::Room_Scene;
	}
	//��Ʈ��ũ Ŭ������ ������ִ� �� ��� ������ ���� �ٸ� ���Ӿ���ȯ
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
	//��Ŷ ����ü�ȿ� ���� ��ư�� � ���������� ������ �ʿ䰡 ����. 
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

