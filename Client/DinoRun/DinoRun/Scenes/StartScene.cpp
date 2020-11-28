#include "StartScene.h"
#include "../Common/FrameWork/GameManager.h"
#include "../Common/FrameWork/NetWorkManager.h"
#include "../Common/FrameWork/SoundManager.h"
#include "EventHandler/EventHandler.h"

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
void StartScene::BuildObjects()
{
	m_pd3dCommandList = GameManager::GetInstance()->GetCommandList().Get();

	SoundManager::GetInstance()->Play("Start_BGM", 0.2f);

	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name = "Resources/Images/T_LoginBackGround.dds";
	uiShader->BuildObjects(&name);
	instacingUiShaders.emplace_back(uiShader);

	UI_INFO button_info;
	button_info.textureName = "Resources/Images/T_Button.dds";
	button_info.meshSize = XMFLOAT2(0.15f, 0.12f);
	button_info.positions.emplace_back(XMFLOAT3(0.31f, -0.5f, 0.0f));
	button_info.f_uvY.emplace_back(0.25f);
	button_info.maxUv = XMFLOAT2(0.5f, 0.25f);
	button_info.minUv = XMFLOAT2(0.0f, 0.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(&button_info);
	instacingUiShaders.emplace_back(uiShader);

	
	gameTexts.emplace_back(GameText(XMFLOAT2(0.27f, 0.60f)));// ID구간
	gameTexts.emplace_back(GameText(XMFLOAT2(0.27f, 0.75f)));// PassWord구간

	GameManager::GetInstance()->RenderLoading();
	GameManager::GetInstance()->RenderLoading();
	GameManager::GetInstance()->RenderLoading();
	GameManager::GetInstance()->RenderLoading();
	GameManager::GetInstance()->RenderLoading();
	GameManager::GetInstance()->RenderLoading();
	GameManager::GetInstance()->RenderLoading();
	GameManager::GetInstance()->RenderLoading();
	CreateShaderVariables();
}

void StartScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	if (sceneType != SceneType::Start_Scene)
		return;

	Point2D point;
	::SetCapture(hWnd);
	::GetCursorPos(&m_ptOldCursorPos);
	ScreenToClient(hWnd, &m_ptOldCursorPos);
	point = ScreenToProj(EventHandler::GetInstance()->m_nWndClientWidth, EventHandler::GetInstance()->m_nWndClientHeight, m_ptOldCursorPos);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		if (point.x > 0.16f && point.x < 0.46f && point.y > -0.62f && point.y < -0.38f) //로그인 버튼 충돌체크
		{
			if (!isClickedLogin)
			{
				BUTTON_SHADER->getUvXs()[0] = 0.5f;
				isClickedLogin = true;
			}
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
				BUTTON_SHADER->getUvXs()[0] = 0.0f;
				isClickedLogin = false;

				gameTexts[PASSWORD].text; //패스워드  
				gameTexts[ID].text;  //아이디  둘 다 영문   string자료형임.
				
				m_sPlayerId = gameTexts[ID].text;
				NetWorkManager::GetInstance()->SetPlayerName(m_sPlayerId);
#ifndef isConnectedToServer
				sceneType = Room_Scene;
#else
				NetWorkManager::GetInstance()->SetConnectState(NetWorkManager::CONNECT_STATE::TRY); // 연결상태를 TRY로 하여 NetWorkManager::GetInstance()->ConnecttoServer호출
				NetWorkManager::GetInstance()->LoadToServer(hWnd);
				sceneType = Room_Scene;
				NetWorkManager::GetInstance()->SetRoomNum(0);
				NetWorkManager::GetInstance()->SetGameMode(0);
#endif
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
	if (sceneType != SceneType::Start_Scene)
		return;

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

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_FONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList, m_pCamera, gameTexts);

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
	if (sceneType != SceneType::Start_Scene)
	{
		SoundManager::GetInstance()->AllStop();
		return sceneType;
	}

	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);
	return SceneType::Start_Scene;
}


void StartScene::CreateShaderVariables()
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


void StartScene::ProcessPacket(char* packet, float fTimeElapsed)
{
	switch (packet[1])
	{
	case SC_ACCESS_COMPLETE: 
		UpdateLogin(packet, fTimeElapsed);  //이 함수가 호출 되면 다음 프레임에 로비씬으로 넘어가게 됨.
		break;
	default: // 로그인 실패같은 경우에는 무시함.
		break;
	}
}

void StartScene::UpdateLogin(char* packet, float fTimeElapsed)
{
	SC_PACKET_ACCESS_COMPLETE* accessInfo = reinterpret_cast<SC_PACKET_ACCESS_COMPLETE*>(packet);

	m_sPlayerId = gameTexts[ID].text;
	NetWorkManager::GetInstance()->SetMyID(accessInfo->myId);

	sceneType = Room_Scene;
	NetWorkManager::GetInstance()->SetRoomNum(0);
	NetWorkManager::GetInstance()->SetGameMode(0);
}

