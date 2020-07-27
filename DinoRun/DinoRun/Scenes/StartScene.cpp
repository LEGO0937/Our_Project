#include "StartScene.h"
#include "../Common/FrameWork/CreateManager.h"
#include "../Common/FrameWork/NetWorkManager.h"
#include "../Common/FrameWork/SoundManager.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"

#define ID 0
#define PASSWORD 1

#define BUTTON_SHADER instacingUiShaders[1]

StartScene::StartScene():BaseScene()
{
	sceneType = SceneType::Start_Scene;
}
StartScene::~StartScene()
{
	SoundManager::GetInstance()->Stop("Start_BGM");
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

	SoundManager::GetInstance()->Play("Start_BGM", 0.2f);

	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name = "Resources/Images/T_LoginBackGround.dds";
	uiShader->BuildObjects(pCreateManager.get(), &name);
	instacingUiShaders.emplace_back(uiShader);

	UI_INFO button_info;
	button_info.textureName = "Resources/Images/T_Button.dds";
	button_info.meshSize = XMFLOAT2(0.15f, 0.12f);
	button_info.positions.emplace_back(XMFLOAT3(0.31f, -0.5f, 0.0f));
	button_info.f_uvY.emplace_back(0.25f);
	button_info.maxUv = XMFLOAT2(0.5f, 0.25f);
	button_info.minUv = XMFLOAT2(0.0f, 0.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &button_info);
	instacingUiShaders.emplace_back(uiShader);

	
	gameTexts.emplace_back(GameText(XMFLOAT2(0.27f, 0.60f)));// ID구간
	gameTexts.emplace_back(GameText(XMFLOAT2(0.27f, 0.75f)));// PassWord구간

	m_pCreateManager->RenderLoading();
	m_pCreateManager->RenderLoading();
	m_pCreateManager->RenderLoading();
	m_pCreateManager->RenderLoading();
	m_pCreateManager->RenderLoading();
	m_pCreateManager->RenderLoading();
	m_pCreateManager->RenderLoading();
	m_pCreateManager->RenderLoading();
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
		if (point.x > 0.16f && point.x < 0.46f && point.y > -0.62f && point.y < -0.38f) //로그인 버튼 충돌체크
		{
			BUTTON_SHADER->getUvXs()[0] = 0.5f;
			isClickedLogin = true;
		}
		else if (point.x > -0.48f && point.x < 0.13f && point.y > -0.31f && point.y < -0.2f) //ID TEXT 충돌체크
		{
			isClickedID = true;
			isClickedPassWord = false;
		}
		else if (point.x > -0.48f && point.x < 0.13f && point.y > -0.63f && point.y < -0.5f) //PassWord TEXT 충돌체크
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
		break;
	case WM_LBUTTONUP:
		if (point.x > 0.16f && point.x < 0.46f && point.y > -0.62f && point.y < -0.38f) //로그인 버튼 충돌체크
		{
			if (isClickedLogin)
			{
				//*서버*
				//이 구간에서 서버와 연결하여 아이디와 패스워드를 보냄, 서버에서 확인후 존재하는 아이디가 맞다면
				//닉네임을 보내줌, 없을 시 ""빈 문자열(string형으로 받을 것)
				// 닉네임을 받았다면 네트워크 클래스에 닉네임 저장 후, Lobbyscene으로 이동.
				//씬 전환 
				//m_sPlayerId = 로그인 성공 시 서버로부터 닉네임 받음;
				
				
				//순서
				//아이디와 비밀번호가 있는 패킷을 send
				//recv로 로그인 성공인지 실패인지 확인.
				//로그인 성공시에는 m_sPlayerId에 닉네임을 입력하고 로비씬으로 넘어감.
				m_sPlayerId = gameTexts[ID].text;
				NetWorkManager::GetInstance()->SetPlayerName(m_sPlayerId);
				sceneType = Lobby_Scene;
			}
		}
		else
		{
			if (isClickedLogin)
			{
				isClickedLogin = false;
				BUTTON_SHADER->getUvXs()[0] = 0.0f;
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
	case WM_CHAR:
		if (isalnum((TCHAR)wParam))
		{
			if (isClickedID)
			{
				if (gameTexts[ID].text.size() < 8)
					gameTexts[ID].text.push_back((TCHAR)wParam);
			}
			else if (isClickedPassWord)
			{
				if (gameTexts[PASSWORD].text.size() < 8)
					gameTexts[PASSWORD].text.push_back((TCHAR)wParam);
			}
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			sceneType = SceneType::Default_Scene;			
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_BACK:
			if (isClickedID && gameTexts[ID].text.length() > 0)
				gameTexts[ID].text.pop_back();
						
			if (isClickedPassWord && gameTexts[PASSWORD].text.length() > 0)
				gameTexts[PASSWORD].text.pop_back();
			break;

		default:
			break;
		}
		break;
	
	default:
		break;
	}
}
void StartScene::ProcessInput(HWND hwnd, float deltaTime)
{
	
}

void StartScene::Render()
{
	BaseScene::Render();

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
	//말 그대로 애니메이션 update

}

SceneType StartScene::Update(CreateManager* pCreateManager, float fTimeElapsed)
{
	//물리 및 충돌을 위한 update
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


void StartScene::ProcessPacket(char* packet)
{

}