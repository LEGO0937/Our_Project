#include "StartScene.h"
#include "../Common/FrameWork/CreateManager.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"

StartScene::StartScene():BaseScene()
{
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

	ComPtr<ID3D12Device> m_pd3dDevice = pCreateManager->GetDevice();
	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	CObInstancingShader* shader;
	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name = "Resources/Images/LoginBackGround.dds";
	uiShader->BuildObjects(pCreateManager, &name);
	instacingUiShaders.emplace_back(uiShader);

	UI_INFO button_info;
	button_info.textureName = "Resources/Images/Button.dds";
	button_info.meshSize = XMFLOAT2(0.15, 0.12);
	button_info.positions.emplace_back(XMFLOAT3(0.31, -0.5, 0));
	button_info.f_uvY.emplace_back(0.25);
	
	uiShader = new ButtonShader;
	uiShader->BuildObjects(pCreateManager, &button_info);
	instacingUiShaders.emplace_back(uiShader);

	
	gameTexts.emplace_back(GameText(XMFLOAT2(0.27,0.60)));// ID구간
	gameTexts.emplace_back(GameText(XMFLOAT2(0.27, 0.75)));// PassWord구간

	CreateShaderVariables(pCreateManager);
}

void StartScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	Point2D point;
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		ScreenToClient(hWnd,&m_ptOldCursorPos);
		point = ScreenToProj(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_ptOldCursorPos);
		if (point.x > 0.16 && point.x < 0.46 && point.y > -0.62 && point.y < -0.38) //로그인 버튼 충돌체크
		{
			instacingUiShaders[1]->getUvXs()[0] = 0.5;
			isClickedLogin = true;
		}
		else if (point.x > -0.48 && point.x < 0.13 && point.y > -0.31 && point.y < -0.2) //ID TEXT 충돌체크
		{
			isClickedID = true;
			isClickedPassWord = false;
		}
		else if (point.x > -0.48 && point.x < 0.13 && point.y > -0.63 && point.y < -0.5) //PassWord TEXT 충돌체크
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
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		::GetCursorPos(&m_ptOldCursorPos);
		ScreenToClient(hWnd, &m_ptOldCursorPos);
		point = ScreenToProj(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_ptOldCursorPos);
		if (point.x > 0.16 && point.x < 0.46 && point.y > -0.62 && point.y < -0.38) //로그인 버튼 충돌체크
		{
			if (isClickedLogin)
			{
				//씬 전환
			}
		}
		else
		{
			if (isClickedLogin)
			{
				isClickedLogin = false;
				instacingUiShaders[1]->getUvXs()[0] = 0.0;
			}
		}
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
	/*키보드의 상태 정보를 반환한다. 화살표 키(‘→’, ‘←’, ‘↑’, ‘↓’)를 누르면 플레이어를 오른쪽/왼쪽(로컬 x-축), 앞/
	뒤(로컬 z-축)로 이동한다. ‘Page Up’과 ‘Page Down’ 키를 누르면 플레이어를 위/아래(로컬 y-축)로 이동한다.*/
	if (::GetKeyboardState(pKeyBuffer))
	{
		// 찜
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
	/*마우스를 캡쳐했으면 마우스가 얼마만큼 이동하였는 가를 계산한다. 마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의
	메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다. 그러므로 마우스가 캡쳐된
	것은 마우스 버튼이 눌려진 상태를 의미한다. 마우스 버튼이 눌려진 상태에서 마우스를 좌우 또는 상하로 움직이면 플
	레이어를 x-축 또는 y-축으로 회전한다.*/
	//if (::GetCapture() == hwnd)
	//{
	//	//마우스 커서를 화면에서 없앤다(보이지 않게 한다).
	//	::SetCursor(NULL);
	//	//현재 마우스 커서의 위치를 가져온다. 
	//	::GetCursorPos(&ptCursorPos);
	//	//마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다. 
	//	cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
	//	cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
	//	//마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정한다. 
	//	//m_ptOldCursorPos = ptCursorPos;
	//	::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	//}
	//마우스 또는 키 입력이 있으면 플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			/*cxDelta는 y-축의 회전을 나타내고 cyDelta는 x-축의 회전을 나타낸다. 오른쪽 마우스 버튼이 눌려진 경우
			cxDelta는 z-축의 회전을 나타낸다.*/
		}
		/*플레이어를 dwDirection 방향으로 이동한다(실제로는 속도 벡터를 변경한다).
		이동 거리는 시간에 비례하도록 한다. 플레이어의 이동 속력은 (50/초)로 가정한다.*/

	}
	//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다. 

}

void StartScene::Render()
{
	BaseScene::Render();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[3]);
	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[16]);
	if (instacingUiShaders[0])
		instacingUiShaders[0]->Render(m_pd3dCommandList.Get(), m_pCamera);
	if (instacingUiShaders[1])
		instacingUiShaders[1]->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[15]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList.Get(), m_pCamera, gameTexts);

#ifdef _WITH_BOUND_BOX

#endif
}


void StartScene::AnimateObjects(float fTimeElapsed)
{
	//말 그대로 애니메이션 update

}

SceneType StartScene::Update(float fTimeElapsed)
{
	//물리 및 충돌을 위한 update

	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);
	return Lobby_Scene;
}


void StartScene::CreateShaderVariables(shared_ptr<CreateManager> pCreateManager)
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