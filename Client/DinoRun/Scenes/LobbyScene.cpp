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

	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.28),XMFLOAT2(1.05,1.05)));  //유저 목록 8줄
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.35),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.42),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.49),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.56),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.63),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.70),XMFLOAT2(1.05,1.05)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71, 0.77),XMFLOAT2(1.05,1.05)));

	gameTexts.emplace_back(GameText(XMFLOAT2(0.09, 0.24), XMFLOAT2(0.8, 0.8)));  //방 인원 수
	gameTexts.emplace_back(GameText(XMFLOAT2(0.09, 0.43), XMFLOAT2(0.8, 0.8)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.40, 0.24), XMFLOAT2(0.8, 0.8)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.40, 0.43), XMFLOAT2(0.8, 0.8)));

	UI_INFO view_info;    //게임중 or 대기중 뷰
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


	view_info.textureName = "Resources/Images/Arrow_Button.dds";  //화살표 버튼
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
		if (point.x > -0.48 && point.x < -0.32 && point.y > -0.36 && point.y < -0.24) //방목록 왼쪽화살표 충돌체크
		{
			instacingUiShaders[2]->getUvXs()[0] = 0.5;
			isClickedLeftRoom = true;
		}
		else if (point.x > -0.26 && point.x < -0.1 && point.y > -0.36 && point.y < -0.24) //방목록 오른쪽 화살표 충돌체크
		{
			instacingUiShaders[2]->getUvXs()[1] = 0.5;
			isClickedRightRoom = true;
		}
		else if (point.x > 0.47 && point.x < 0.63 && point.y > -0.81 && point.y < -0.69) //유저목록 왼쪽화살표 충돌체크
		{
			instacingUiShaders[2]->getUvXs()[2] = 0.5;
			isClickedLeftUser = true;
		}
		else if (point.x > 0.69 && point.x < 0.85 && point.y > -0.81 && point.y < -0.69) //유저목록 오른쪽화살표 충돌체크
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
				//더블 클릭 구간 
				if (point.x > -0.87 && point.x < -0.34 && point.y > 0.22 && point.y < 0.51) //1번 방 충돌체크
				{
					sceneType = SceneType::Room_Scene;
				}
				else if (point.x > -0.24 && point.x < 0.28 && point.y > 0.22 && point.y < 0.51) //2번 방 충돌체크
				{
					sceneType = SceneType::Room_Scene;
				}
				else if (point.x > -0.87 && point.x < -0.34 && point.y > -0.17 && point.y < 0.14) //3번 방 충돌체크
				{
					sceneType = SceneType::Room_Scene;
				}
				else if (point.x > -0.24 && point.x < 0.28 && point.y > -0.17 && point.y < 0.14) //4번 방 충돌체크
				{
					sceneType = SceneType::Room_Scene;
				}
			}
		}
		break;
	case WM_RBUTTONDOWN:
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		if (point.x > -0.32 && point.x < -0.48 && point.y > -0.36 && point.y < -0.24) //방목록 왼쪽화살표 충돌체크
		{
			if (isClickedLeftRoom)
			{
				//방목록 변경
			}
					
		}
		else if (point.x > -0.26 && point.x < -0.1 && point.y > -0.36 && point.y < -0.24) //방목록 오른쪽 화살표 충돌체크
		{
			if (isClickedRightRoom)
			{
				//방목록 변경
			}
		}
		else if (point.x > 0.47 && point.x < 0.63 && point.y > -0.81 && point.y < -0.69) //유저목록 왼쪽화살표 충돌체크
		{
			if (isClickedLeftUser)
			{
				//유저목록 변경
			}
		}
		else if (point.x > 0.69 && point.x < 0.85 && point.y > -0.81 && point.y < -0.69) //유저목록 오른쪽화살표 충돌체크
		{
			if (isClickedRightUser)
			{
				//유저목록 변경
				
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
		//마우스 캡쳐를 해제한다. 

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
	if (::GetCapture() == hwnd)
	{
		////마우스 커서를 화면에서 없앤다(보이지 않게 한다).
		//::SetCursor(NULL);
		////현재 마우스 커서의 위치를 가져온다. 
		//::GetCursorPos(&ptCursorPos);
		////마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다. 
		//cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		//cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		////마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정한다. 
		////m_ptOldCursorPos = ptCursorPos;
		//::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}
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
	//말 그대로 애니메이션 update

}

SceneType LobbyScene::Update(float fTimeElapsed)
{
	if (sceneType != SceneType::Lobby_Scene)
	{
		return sceneType;
	}

	//물리 및 충돌을 위한 update
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