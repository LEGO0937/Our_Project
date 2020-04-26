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
void LobbyScene::BuildObjects(shared_ptr<CreateManager> pCreateManager)
{

	ComPtr<ID3D12Device> m_pd3dDevice = pCreateManager->GetDevice();
	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	CObInstancingShader* shader;
	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name = "Resources/Images/LobbyBackGround.dds";
	uiShader->BuildObjects(pCreateManager, &name);
	instacingUiShaders.emplace_back(uiShader);

	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.28),XMFLOAT2(1.05,1.05)));  //���� ��� 8��
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.35),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.42),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.49),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.56),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.63),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.70),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.77),XMFLOAT2(1.05,1.05)));

	gameTexts.emplace_back(GameText(XMFLOAT2(0.09, 0.24), XMFLOAT2(0.8, 0.8)));  //�� �ο� ��
	gameTexts.emplace_back(GameText(XMFLOAT2(0.09, 0.43), XMFLOAT2(0.8, 0.8)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.40, 0.24), XMFLOAT2(0.8, 0.8)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.40, 0.43), XMFLOAT2(0.8, 0.8)));

	UI_INFO view_info;    //������ or ����� ��
	view_info.textureName = "Resources/Images/Button.dds";
	view_info.meshSize = XMFLOAT2(0.087, 0.05);
	view_info.positions.emplace_back(XMFLOAT3(-0.45, 0.475, 0));
	view_info.f_uvY.emplace_back(0.5);

	view_info.positions.emplace_back(XMFLOAT3(-0.45, 0.087, 0));
	view_info.f_uvY.emplace_back(0.5);

	view_info.positions.emplace_back(XMFLOAT3(0.175, 0.475, 0));
	view_info.f_uvY.emplace_back(0.5);

	view_info.positions.emplace_back(XMFLOAT3(0.175, 0.087, 0));
	view_info.f_uvY.emplace_back(0.5);

	uiShader = new ButtonShader;
	uiShader->BuildObjects(pCreateManager, &view_info);
	instacingUiShaders.emplace_back(uiShader);


	view_info.textureName = "Resources/Images/Arrow_Button.dds";  //ȭ��ǥ ��ư
	view_info.meshSize = XMFLOAT2(0.08, 0.06);
	view_info.positions.clear();
	view_info.f_uvY.clear();
	view_info.positions.emplace_back(XMFLOAT3(-0.40, -0.3, 0));
	view_info.f_uvY.emplace_back(0.25);
	view_info.positions.emplace_back(XMFLOAT3(-0.18, -0.3, 0));
	view_info.f_uvY.emplace_back(0);

	view_info.positions.emplace_back(XMFLOAT3(0.55, -0.75, 0));
	view_info.f_uvY.emplace_back(0.25);
	view_info.positions.emplace_back(XMFLOAT3(0.77, -0.75, 0));
	view_info.f_uvY.emplace_back(0);

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
		if (point.x > -0.48 && point.x < -0.32 && point.y > -0.36 && point.y < -0.24) //���� ����ȭ��ǥ �浹üũ
		{
			instacingUiShaders[2]->getUvXs()[0] = 0.5;
			isClickedLeftRoom = true;
		}
		else if (point.x > -0.26 && point.x < -0.1 && point.y > -0.36 && point.y < -0.24) //���� ������ ȭ��ǥ �浹üũ
		{
			instacingUiShaders[2]->getUvXs()[1] = 0.5;
			isClickedRightRoom = true;
		}
		else if (point.x > 0.47 && point.x < 0.63 && point.y > -0.81 && point.y < -0.69) //������� ����ȭ��ǥ �浹üũ
		{
			instacingUiShaders[2]->getUvXs()[2] = 0.5;
			isClickedLeftUser = true;
		}
		else if (point.x > 0.69 && point.x < 0.85 && point.y > -0.81 && point.y < -0.69) //������� ������ȭ��ǥ �浹üũ
		{
			instacingUiShaders[2]->getUvXs()[3] = 0.5;
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
				if (point.x > -0.87 && point.x < -0.34 && point.y > 0.22 && point.y < 0.51) //1�� �� �浹üũ
				{
					sceneType = SceneType::Room_Scene;
				}
				else if (point.x > -0.24 && point.x < 0.28 && point.y > 0.22 && point.y < 0.51) //2�� �� �浹üũ
				{
					sceneType = SceneType::Room_Scene;
				}
				else if (point.x > -0.87 && point.x < -0.34 && point.y > -0.17 && point.y < 0.14) //3�� �� �浹üũ
				{
					sceneType = SceneType::Room_Scene;
				}
				else if (point.x > -0.24 && point.x < 0.28 && point.y > -0.17 && point.y < 0.14) //4�� �� �浹üũ
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
		if (point.x > -0.32 && point.x < -0.48 && point.y > -0.36 && point.y < -0.24) //���� ����ȭ��ǥ �浹üũ
		{
			if (isClickedLeftRoom)
			{
				//���� ����
			}
					
		}
		else if (point.x > -0.26 && point.x < -0.1 && point.y > -0.36 && point.y < -0.24) //���� ������ ȭ��ǥ �浹üũ
		{
			if (isClickedRightRoom)
			{
				//���� ����
			}
		}
		else if (point.x > 0.47 && point.x < 0.63 && point.y > -0.81 && point.y < -0.69) //������� ����ȭ��ǥ �浹üũ
		{
			if (isClickedLeftUser)
			{
				//������� ����
			}
		}
		else if (point.x > 0.69 && point.x < 0.85 && point.y > -0.81 && point.y < -0.69) //������� ������ȭ��ǥ �浹üũ
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
		switch (wParam)
		{
		default:
			break;
		}
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
	POINT ptCursorPos;
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
		if (shader) shader->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	for (CUiShader* shader : instacingUiShaders)
	{
		if(shader)
			shader->Render(m_pd3dCommandList.Get(), m_pCamera);
	}
	
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList.Get(), m_pCamera, gameTexts);

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


void LobbyScene::CreateShaderVariables(shared_ptr<CreateManager> pCreateManager)
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