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
	if (sceneType != SceneType::Start_Scene)
		return;

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
				//*서버*
				BUTTON_SHADER->getUvXs()[0] = 0.0f;
				isClickedLogin = false;

				gameTexts[PASSWORD].text; //패스워드  
				gameTexts[ID].text;  //아이디  둘 다 영문   string자료형임.
				//순서
				//아이디와 비밀번호가 있는 패킷을 send
				//recv로 로그인 성공인지 실패인지 확인.
				//로그인 성공시에는 아래 세줄 수행 후 로비씬으로 넘어감.
				//패킷 구현 후에는 아래 세줄이 프로세스 처리 함수중 로그인 성공함수에 들어갈 예정->UpdateLogin()
				m_sPlayerId = gameTexts[ID].text;
				NetWorkManager::GetInstance()->SetPlayerName(m_sPlayerId);
#ifdef noLobby
				sceneType = Room_Scene;
#else
				sceneType = Lobby_Scene;
#endif
				// 바로 위의 코드는 나중에 프로세스 패킷에서 처리하는 함수로 만들어졌다
				// 추후에 삭제
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


void StartScene::ProcessPacket(char* packet, float fTimeElapsed)
{
	switch (packet[1])
	{
	case SC_ACCESS_COMPLETE:   //케이스는 로그인용으로 따로 만들어줄 것.
		UpdateLogin(packet, fTimeElapsed);  //이 함수가 호출 되면 다음 프레임에 로비씬으로 넘어가게 됨.
		break;
	default: // 로그인 실패같은 경우에는 무시함.
		break;
	}
}

void StartScene::UpdateLogin(char* packet, float fTimeElapsed)
{
	m_sPlayerId = gameTexts[ID].text;
	NetWorkManager::GetInstance()->SetPlayerName(m_sPlayerId); // 플레이어 아이디 설정
#ifdef noLobby
	sceneType = Room_Scene;
	NetWorkManager::GetInstance()->SetGameMode(0);
#else
	sceneType = Lobby_Scene;
#endif

	//그리고 여기서 클라이언트 넘버를 받는게 맞다면
	//NetWorkManager::GetInstance()->SetMyID()로 아이디 적용할것.
}



//NetWorkManager::GetInstance()->SetServerIP("127.0.0.1"); // IP를 통한 연결 필요, 나중에 다른 컴에 접속을 요구할거면 ipconfig로 ip주소 따서 진행
//NetWorkManager::GetInstance()->SetConnectState(NetWorkManager::CONNECT_STATE::TRY); // 연결상태를 TRY로 하여 NetWorkManager::GetInstance()->ConnecttoServer호출
// 바로 위 함수는 CG프레임워크에 ConnectingServer호출에 필요한 놈
// 과연 어떤 자리에 초기 위치를 설정해야될까 ㅇㅁㅇ?


