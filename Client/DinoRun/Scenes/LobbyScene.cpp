#include "LobbyScene.h"
#include "../Common/FrameWork/CreateManager.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"

LobbyScene::LobbyScene() :BaseScene()
{
	sceneType = SceneType::Lobby_Scene;
}
LobbyScene::~LobbyScene()
{

}
void LobbyScene::ReleaseUploadBuffers()
{
	BaseScene::ReleaseUploadBuffers();
	
	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) shader->ReleaseUploadBuffers();
	
	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
}
void LobbyScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();
	

	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	
	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	
	instacingBillBoardShaders.clear();

}
void LobbyScene::BuildObjects(CreateManager* pCreateManager)
{
	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name = "Resources/Images/LobbyBackGround.dds";
	uiShader->BuildObjects(pCreateManager, &name);
	instacingUiShaders.emplace_back(uiShader);

	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.28f),XMFLOAT2(1.05f,1.05f)));  //���� ��� 8��
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.35f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.42f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.49f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.56f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.63f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.70f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.77f),XMFLOAT2(1.05f,1.05f)));

	gameTexts.emplace_back(GameText(XMFLOAT2(0.09f, 0.24f), XMFLOAT2(0.8f, 0.8f)));  //�� �ο� ��
	gameTexts.emplace_back(GameText(XMFLOAT2(0.09f, 0.43f), XMFLOAT2(0.8f, 0.8f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.40f, 0.24f), XMFLOAT2(0.8f, 0.8f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.40f, 0.43f), XMFLOAT2(0.8f, 0.8f)));

	UI_INFO view_info;    //������ or ����� ��
	view_info.textureName = "Resources/Images/Button.dds";
	view_info.meshSize = XMFLOAT2(0.087f, 0.05f);
	view_info.positions.emplace_back(XMFLOAT3(-0.45f, 0.475f, 0.0f));
	view_info.f_uvY.emplace_back(0.5f);

	view_info.positions.emplace_back(XMFLOAT3(-0.45f, 0.087f, 0.0f));
	view_info.f_uvY.emplace_back(0.5f);

	view_info.positions.emplace_back(XMFLOAT3(0.175f, 0.475f, 0.0f));
	view_info.f_uvY.emplace_back(0.5f);

	view_info.positions.emplace_back(XMFLOAT3(0.175f, 0.087f, 0.0f));
	view_info.f_uvY.emplace_back(0.5f);

	uiShader = new ButtonShader;
	uiShader->BuildObjects(pCreateManager, &view_info);
	instacingUiShaders.emplace_back(uiShader);


	view_info.textureName = "Resources/Images/Arrow_Button.dds";  //ȭ��ǥ ��ư
	view_info.meshSize = XMFLOAT2(0.08f, 0.06f);
	view_info.positions.clear();
	view_info.f_uvY.clear();
	view_info.positions.emplace_back(XMFLOAT3(-0.40f, -0.3f, 0.0f));
	view_info.f_uvY.emplace_back(0.25f);
	view_info.positions.emplace_back(XMFLOAT3(-0.18f, -0.3f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);

	view_info.positions.emplace_back(XMFLOAT3(0.55f, -0.75f, 0.0f));
	view_info.f_uvY.emplace_back(0.25f);
	view_info.positions.emplace_back(XMFLOAT3(0.77f, -0.75f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);

	uiShader = new ButtonShader;
	uiShader->BuildObjects(pCreateManager, &view_info);
	instacingUiShaders.emplace_back(uiShader);


	//fontShader = new FontShader;
	//fontShader->BuildObjects(pCreateManager, NULL);

	CreateShaderVariables(pCreateManager);
}

void LobbyScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
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
		if (point.x > -0.48f && point.x < -0.32f && point.y > -0.36f && point.y < -0.24f) //���� ����ȭ��ǥ �浹üũ
		{
			instacingUiShaders[2]->getUvXs()[0] = 0.5;
			isClickedLeftRoom = true;
		}
		else if (point.x > -0.26f && point.x < -0.1f && point.y > -0.36f && point.y < -0.24f) //���� ������ ȭ��ǥ �浹üũ
		{
			instacingUiShaders[2]->getUvXs()[1] = 0.5f;
			isClickedRightRoom = true;
		}
		else if (point.x > 0.47f && point.x < 0.63f && point.y > -0.81f && point.y < -0.69f) //������� ����ȭ��ǥ �浹üũ
		{
			instacingUiShaders[2]->getUvXs()[2] = 0.5f;
			isClickedLeftUser = true;
		}
		else if (point.x > 0.69f && point.x < 0.85f && point.y > -0.81f && point.y < -0.69f) //������� ������ȭ��ǥ �浹üũ
		{
			instacingUiShaders[2]->getUvXs()[3] = 0.5f;
			isClickedRightUser = true;
		}
		else
		{
			if (!isClicked)
			{
				isClicked = true;
				m_fClickedTime = 0;
			}
			else
			{
				//���� Ŭ�� ���� 
				if (point.x > -0.87f && point.x < -0.34f && point.y > 0.22f && point.y < 0.51f) //1�� �� �浹üũ
				{
					sceneType = SceneType::Room_Scene;
				}
				else if (point.x > -0.24f && point.x < 0.28f && point.y > 0.22f && point.y < 0.51f) //2�� �� �浹üũ
				{
					sceneType = SceneType::Room_Scene;
				}
				else if (point.x > -0.87f && point.x < -0.34f && point.y > -0.17f && point.y < 0.14f) //3�� �� �浹üũ
				{
					sceneType = SceneType::Room_Scene;
				}
				else if (point.x > -0.24f && point.x < 0.28f && point.y > -0.17f && point.y < 0.14f) //4�� �� �浹üũ
				{
					sceneType = SceneType::Room_Scene;
				}
			}
		}
		break;
	case WM_RBUTTONDOWN:
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		if (point.x > -0.32f && point.x < -0.48f && point.y > -0.36f && point.y < -0.24f) //���� ����ȭ��ǥ �浹üũ
		{
			if (isClickedLeftRoom)
			{
				//���� ����
			}
					
		}
		else if (point.x > -0.26f && point.x < -0.1f && point.y > -0.36f && point.y < -0.24f) //���� ������ ȭ��ǥ �浹üũ
		{
			if (isClickedRightRoom)
			{
				//���� ����
			}
		}
		else if (point.x > 0.47f && point.x < 0.63f && point.y > -0.81f && point.y < -0.69f) //������� ����ȭ��ǥ �浹üũ
		{
			if (isClickedLeftUser)
			{
				//������� ����
			}
		}
		else if (point.x > 0.69f && point.x < 0.85f && point.y > -0.81f && point.y < -0.69f) //������� ������ȭ��ǥ �浹üũ
		{
			if (isClickedRightUser)
			{
				//������� ����
				
			}
		}
		else
		{

		}
		isClickedLeftRoom = false;
		isClickedRightRoom = false;

		isClickedLeftUser = false;
		isClickedRightUser = false;
		for(int i = 0; i<4; ++i)
			instacingUiShaders[2]->getUvXs()[i] = 0;
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
void LobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			sceneType = SceneType::Start_Scene;
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
}
void LobbyScene::ProcessInput(HWND hwnd, float deltaTime)
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	/*Ű������ ���� ������ ��ȯ�Ѵ�. ȭ��ǥ Ű(���桯, ���硯, ���衯, ���顯)�� ������ �÷��̾ ������/����(���� x-��), ��/
	��(���� z-��)�� �̵��Ѵ�. ��Page Up���� ��Page Down�� Ű�� ������ �÷��̾ ��/�Ʒ�(���� y-��)�� �̵��Ѵ�.*/
	if (::GetKeyboardState(pKeyBuffer))
	{
		// ��
		if (pKeyBuffer[VK_UP] & 0xF0)
			dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}
	float cxDelta = 0.0f, cyDelta = 0.0f;
	/*���콺�� ĸ�������� ���콺�� �󸶸�ŭ �̵��Ͽ��� ���� ����Ѵ�. ���콺 ���� �Ǵ� ������ ��ư�� ������ ����
	�޽���(WM_LBUTTONDOWN, WM_RBUTTONDOWN)�� ó���� �� ���콺�� ĸ���Ͽ���. �׷��Ƿ� ���콺�� ĸ�ĵ�
	���� ���콺 ��ư�� ������ ���¸� �ǹ��Ѵ�. ���콺 ��ư�� ������ ���¿��� ���콺�� �¿� �Ǵ� ���Ϸ� �����̸� ��
	���̾ x-�� �Ǵ� y-������ ȸ���Ѵ�.*/
	if (::GetCapture() == hwnd)
	{
		////���콺 Ŀ���� ȭ�鿡�� ���ش�(������ �ʰ� �Ѵ�).
		//::SetCursor(NULL);
		////���� ���콺 Ŀ���� ��ġ�� �����´�. 
		//::GetCursorPos(&ptCursorPos);
		////���콺 ��ư�� ���� ���¿��� ���콺�� ������ ���� ���Ѵ�. 
		//cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		//cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		////���콺 Ŀ���� ��ġ�� ���콺�� �������� ��ġ�� �����Ѵ�. 
		////m_ptOldCursorPos = ptCursorPos;
		//::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}
	//���콺 �Ǵ� Ű �Է��� ������ �÷��̾ �̵��ϰų�(dwDirection) ȸ���Ѵ�(cxDelta �Ǵ� cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			/*cxDelta�� y-���� ȸ���� ��Ÿ���� cyDelta�� x-���� ȸ���� ��Ÿ����. ������ ���콺 ��ư�� ������ ���
			cxDelta�� z-���� ȸ���� ��Ÿ����.*/
		}
		/*�÷��̾ dwDirection �������� �̵��Ѵ�(�����δ� �ӵ� ���͸� �����Ѵ�).
		�̵� �Ÿ��� �ð��� ����ϵ��� �Ѵ�. �÷��̾��� �̵� �ӷ��� (50/��)�� �����Ѵ�.*/

	}
	//�÷��̾ ������ �̵��ϰ� ī�޶� �����Ѵ�. �߷°� �������� ������ �ӵ� ���Ϳ� �����Ѵ�. 

}

void LobbyScene::Render(float fTimeElapsed)
{
	BaseScene::Render(fTimeElapsed);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_BILLBOARD]);
	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	for (CUiShader* shader : instacingUiShaders)
	{
		if(shader)
			shader->Render(m_pd3dCommandList, m_pCamera);
	}
	
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList, m_pCamera, gameTexts);

#ifdef _WITH_BOUND_BOX

#endif
}


void LobbyScene::AnimateObjects(float fTimeElapsed)
{
	//�� �״�� �ִϸ��̼� update

}

SceneType LobbyScene::Update(float fTimeElapsed)
{
	if (sceneType != SceneType::Lobby_Scene)
	{
		return sceneType;
	}

	//���� �� �浹�� ���� update
	if (isClicked)
		m_fClickedTime += fTimeElapsed;
	if (m_fClickedTime > 0.3f)
	{
		isClicked = false;
		m_fClickedTime = 0.f;
	}
	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);
	return Lobby_Scene;
}


void LobbyScene::CreateShaderVariables(CreateManager* pCreateManager)
{
}


void LobbyScene::UpdateShaderVariables()
{
}

void LobbyScene::ReleaseShaderVariables()
{

}


void LobbyScene::setPlayer(CPlayer* player)
{
	BaseScene::setPlayer(player);
}

void LobbyScene::setCamera(CCamera* camera)
{
	BaseScene::setCamera(camera);
}