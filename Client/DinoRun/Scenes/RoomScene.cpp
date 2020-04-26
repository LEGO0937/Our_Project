#include "RoomScene.h"

#include "../Common/FrameWork/CreateManager.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"

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


	
	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }

}
void RoomScene::BuildObjects(shared_ptr<CreateManager> pCreateManager)
{

	ComPtr<ID3D12Device> m_pd3dDevice = pCreateManager->GetDevice();
	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	CObInstancingShader* shader;
	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name = "Resources/Images/Room.dds";
	uiShader->BuildObjects(pCreateManager, &name);
	instacingUiShaders.emplace_back(uiShader);

	UI_INFO button_info;
	button_info.textureName = "Resources/Images/Button.dds";
	button_info.meshSize = XMFLOAT2(0.15, 0.12);
	button_info.positions.emplace_back(XMFLOAT3(0.0, -0.8, 0));
	button_info.f_uvY.emplace_back(0);
	//레디 or 준비 상태 확인 인터페이스
	button_info.positions.emplace_back(XMFLOAT3(0.55, 0.57, 0));
	button_info.f_uvY.emplace_back(0);
	button_info.positions.emplace_back(XMFLOAT3(0.55, 0.28, 0));
	button_info.f_uvY.emplace_back(0);
	button_info.positions.emplace_back(XMFLOAT3(0.55, 0.00, 0));
	button_info.f_uvY.emplace_back(0);
	button_info.positions.emplace_back(XMFLOAT3(0.55, -0.28, 0));
	button_info.f_uvY.emplace_back(0);
	button_info.positions.emplace_back(XMFLOAT3(0.55, -0.57, 0));
	button_info.f_uvY.emplace_back(0);

	uiShader = new ButtonShader;
	uiShader->BuildObjects(pCreateManager, &button_info);
	instacingUiShaders.emplace_back(uiShader);


	gameTexts.emplace_back(GameText(XMFLOAT2(0.17, 0.19))); //플레이어 명단
	gameTexts.emplace_back(GameText(XMFLOAT2(0.17, 0.33)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.17, 0.47)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.17, 0.61)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.17, 0.75)));

	CreateShaderVariables(pCreateManager);
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
		if (point.x > -0.15 && point.x < 0.15 && point.y > -0.92 && point.y < -0.68) //로그인 버튼 충돌체크
		{
			if (instacingUiShaders[1]->getUvXs()[0] == 0)
			{
				instacingUiShaders[1]->getUvXs()[0] = 0.5;
				//임시적인 씬이동을 위해 여기서 씬타입 전환
				sceneType = Game_Scene;
			}
			else
				instacingUiShaders[1]->getUvXs()[0] = 0;
		}
		
		break;
	case WM_RBUTTONDOWN:
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다. 
		break;
	case WM_LBUTTONUP:
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

void RoomScene::Render(float fTimeElapsed)
{
	BaseScene::Render(fTimeElapsed);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_BILLBOARD]);
	
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	if (instacingUiShaders[0])
		instacingUiShaders[0]->Render(m_pd3dCommandList.Get(), m_pCamera);
	if (instacingUiShaders[1])
		instacingUiShaders[1]->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList.Get(), m_pCamera, gameTexts);

#ifdef _WITH_BOUND_BOX

#endif
}


void RoomScene::AnimateObjects(float fTimeElapsed)
{
	//말 그대로 애니메이션 update

}

SceneType RoomScene::Update(float fTimeElapsed)
{
	//물리 및 충돌을 위한 update
	if (sceneType != SceneType::Room_Scene)
	{
		return sceneType;
	}

	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);
	return SceneType::Room_Scene;
}


void RoomScene::CreateShaderVariables(shared_ptr<CreateManager> pCreateManager)
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