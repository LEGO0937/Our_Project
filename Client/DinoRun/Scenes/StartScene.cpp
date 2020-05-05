#include "StartScene.h"
#include "../Common/FrameWork/CreateManager.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"

StartScene::StartScene():BaseScene()
{
	sceneType = SceneType::Start_Scene;
}
StartScene::~StartScene()
{

}
void StartScene::ReleaseUploadBuffers()
{
	BaseScene::ReleaseUploadBuffers();

	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) shader->ReleaseUploadBuffers();

	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
}
void StartScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();


	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }

	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }

	instacingBillBoardShaders.clear();

}
void StartScene::BuildObjects(shared_ptr<CreateManager> pCreateManager)
{
	m_pCreateManager = pCreateManager;
	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name = "Resources/Images/LoginBackGround.dds";
	uiShader->BuildObjects(pCreateManager.get(), &name);
	instacingUiShaders.emplace_back(uiShader);

	UI_INFO button_info;
	button_info.textureName = "Resources/Images/Button.dds";
	button_info.meshSize = XMFLOAT2(0.15f, 0.12f);
	button_info.positions.emplace_back(XMFLOAT3(0.31f, -0.5f, 0.0f));
	button_info.f_uvY.emplace_back(0.25f);
	button_info.maxUv = XMFLOAT2(0.5f, 0.25f);
	button_info.minUv = XMFLOAT2(0.0f, 0.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &button_info);
	instacingUiShaders.emplace_back(uiShader);

	
	gameTexts.emplace_back(GameText(XMFLOAT2(0.27f, 0.60f)));// ID����
	gameTexts.emplace_back(GameText(XMFLOAT2(0.27f, 0.75f)));// PassWord����

	CreateShaderVariables(pCreateManager.get());
}

void StartScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
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
		if (point.x > 0.16f && point.x < 0.46f && point.y > -0.62f && point.y < -0.38f) //�α��� ��ư �浹üũ
		{
			instacingUiShaders[1]->getUvXs()[0] = 0.5f;
			isClickedLogin = true;
		}
		else if (point.x > -0.48f && point.x < 0.13f && point.y > -0.31f && point.y < -0.2f) //ID TEXT �浹üũ
		{
			isClickedID = true;
			isClickedPassWord = false;
		}
		else if (point.x > -0.48f && point.x < 0.13f && point.y > -0.63f && point.y < -0.5f) //PassWord TEXT �浹üũ
		{
			isClickedID = false;
			isClickedPassWord = true;
		}
		else
		{
			isClickedID = false;
			isClickedPassWord = false;
		}
		break;
	case WM_RBUTTONDOWN:
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�. 
		break;
	case WM_LBUTTONUP:
		if (point.x > 0.16f && point.x < 0.46f && point.y > -0.62f && point.y < -0.38f) //�α��� ��ư �浹üũ
		{
			if (isClickedLogin)
			{
				//���� ���� �� �� �������� ������ �����Ͽ� ���̵�� �н����尡 ��ġ�ϸ� �� ��ȯ
				//�� ��ȯ 
				//m_sPlayerId = �α��� ���� �� �����κ��� �г��� ����;
				sceneType = Lobby_Scene;
			}
		}
		else
		{
			if (isClickedLogin)
			{
				isClickedLogin = false;
				instacingUiShaders[1]->getUvXs()[0] = 0.0f;
			}
		}
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
void StartScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_BACK:
			if (isClickedID && gameTexts[0].text.length() > 0)
				gameTexts[0].text.pop_back();
						
			if (isClickedPassWord && gameTexts[1].text.length() > 0)
				gameTexts[1].text.pop_back();
			break;

		default:
			if (isalnum((TCHAR)wParam))
			{
				if (isClickedID)
					gameTexts[0].text.push_back((TCHAR)wParam);
				else if (isClickedPassWord)
					gameTexts[1].text.push_back((TCHAR)wParam);
			}
			break;
		}
		break;
	default:
		break;
	}
}
void StartScene::ProcessInput(HWND hwnd, float deltaTime)
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
	//if (::GetCapture() == hwnd)
	//{
	//	//���콺 Ŀ���� ȭ�鿡�� ���ش�(������ �ʰ� �Ѵ�).
	//	::SetCursor(NULL);
	//	//���� ���콺 Ŀ���� ��ġ�� �����´�. 
	//	::GetCursorPos(&ptCursorPos);
	//	//���콺 ��ư�� ���� ���¿��� ���콺�� ������ ���� ���Ѵ�. 
	//	cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
	//	cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
	//	//���콺 Ŀ���� ��ġ�� ���콺�� �������� ��ġ�� �����Ѵ�. 
	//	//m_ptOldCursorPos = ptCursorPos;
	//	::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	//}
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

void StartScene::Render(float fTimeElapsed)
{
	BaseScene::Render(fTimeElapsed);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_BILLBOARD]);
	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);

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


void StartScene::AnimateObjects(float fTimeElapsed)
{
	//�� �״�� �ִϸ��̼� update

}

SceneType StartScene::Update(CreateManager* pCreateManager, float fTimeElapsed)
{
	//���� �� �浹�� ���� update
	if (sceneType != SceneType::Start_Scene)
	{
		return sceneType;
	}

	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);
	return SceneType::Start_Scene;
}


void StartScene::CreateShaderVariables(CreateManager* pCreateManager)
{
}


void StartScene::UpdateShaderVariables()
{
}

void StartScene::ReleaseShaderVariables()
{

}


void StartScene::setPlayer(CPlayer* player)
{
	BaseScene::setPlayer(player);
}

void StartScene::setCamera(CCamera* camera)
{
	BaseScene::setCamera(camera);
}