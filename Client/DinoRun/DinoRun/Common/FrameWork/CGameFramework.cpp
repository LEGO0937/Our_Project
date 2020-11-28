#include "../stdafx.h"
#include "CGameFramework.h"
#include "TerrainObject.h"

#include "GameManager.h"
#include "SoundManager.h"
#include "NetworkManager.h"

#include "EventHandler/EventHandler.h"

#include "../Scenes/GameScene.h"
#include "../Scenes/ItemGameScene.h"
#include "../Scenes/StartScene.h"
#include "../Scenes/LoadingScene.h"
#include "../Scenes/RoomScene.h"
#include "../Scenes/EndScene.h"
ID3D12PipelineState** CGameFramework::m_ppd3dPipelineStates = NULL;

CGameFramework::CGameFramework()
{
	EventHandler::GetInstance()->Update();
	SoundManager::GetInstance()->Initialize();
}

CGameFramework::~CGameFramework()
{
	EventHandler::GetInstance()->destroy();
	NetWorkManager::GetInstance()->Release();
	NetWorkManager::GetInstance()->destroy();
	SoundManager::GetInstance()->destroy();
	GameManager::GetInstance()->destroy();
}

bool CGameFramework::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hWnd = hWnd;
#ifdef isConnectedToServer
	NetWorkManager::GetInstance()->SetHwnd(hWnd);
#endif
	GameManager::GetInstance()->Initialize(hInstance, hWnd);

	::gnCbvSrvDescriptorIncrementSize = GameManager::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_pCamera = new CMinimapCamera;
	m_pCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pCamera->GenerateOrthoProjectionMatrix(1000, 1000, 10, 300.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	m_pCamera->GenerateViewMatrix(XMFLOAT3(0, 0, 0), XMFLOAT3(128 * TerrainScaleX, 0, 128 * TerrainScaleZ), XMFLOAT3(0, 0, 1));
	m_pCamera->CreateShaderVariables(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());
	
	
	return(true);
}
void CGameFramework::Release()
{
	ReleaseObjects();

	if (m_pLoadingScene)
	{
		m_pLoadingScene->ReleaseObjects();
		m_pLoadingScene->ReleaseShaderVariables();
		m_pLoadingScene.reset();
		m_pLoadingScene = NULL;
	}

	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i])
			m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}

	if (m_pCamera)
	{
		m_pCamera->ReleaseShaderVariables();
		delete m_pCamera;
	}
}

void LoadingFrameAdvance()
{

}
void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick();
	float fTimeElapsed = m_GameTimer.DeltaTime();

	m_pScene->ProcessInput(m_hWnd, fTimeElapsed);
	m_pScene->FixedUpdate(fTimeElapsed);
	m_pScene->AnimateObjects(fTimeElapsed); //바뀐 행렬값으로 애니메이션 수행
	
	GameManager::GetInstance()->WaitForGpuComplete();
	GameManager::GetInstance()->ResetCommandAllocator();
	GameManager::GetInstance()->ResetCommandList();
	GameManager::GetInstance()->SetComputeRootSignature();
	
	m_CurState = m_pScene->Update(fTimeElapsed);  
	GameManager::GetInstance()->ExecuteCommandList();

	if (m_pScene)
		GameManager::GetInstance()->Render(m_pScene, fTimeElapsed);

	if (m_CurState != m_PrevState)
	{
		ChangeSceneByType(m_CurState);
		m_GameTimer.Tick();
		//씬 교환 부분.
	}
}
string ip;
void CGameFramework::BuildObjects()
{
	
#ifdef isConnectedToServer
	
	AllocConsole();
	HANDLE hInputConsole = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hOutputConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	char buff[32] = { 0 };
	DWORD maxChar = 32;
	DWORD charIn;

	string str = "서버의 Ip를 입력해주세요:";
	WriteConsoleA(hOutputConsole, str.c_str(), str.length(), &charIn, NULL);
	ReadConsoleA(hInputConsole, buff, maxChar - 2, &charIn, NULL);
	buff[charIn - 2] = '\0';
	ip = buff;

	FreeConsole();
	NetWorkManager::GetInstance()->SetServerIP(ip.c_str()); // IP를 통한 연결 필요, 나중에 다른 컴에 접속을 요구할거면 ipconfig로 ip주소 따서 진행
	NetWorkManager::GetInstance()->SetConnectState(NetWorkManager::CONNECT_STATE::NONE); // 연결상태를 TRY로 하여 NetWorkManager::GetInstance()->ConnecttoServer호출


#endif
	BuildPipelineState();
	GameManager::GetInstance()->ResetCommandList();

	m_pFontManager = shared_ptr<FontManager>(new FontManager);
	m_pFontManager->Initialize();
	
	m_pLoadingScene = shared_ptr<LoadingScene>(new LoadingScene());
	m_pLoadingScene->SetGraphicsRootSignature(GameManager::GetInstance()->GetGraphicsRootSignature().Get());
	m_pLoadingScene->SetPipelineStates(m_nPipelineStates, m_ppd3dPipelineStates);
	m_pLoadingScene->BuildObjects();
	m_pLoadingScene->SetFontShader(m_pFontManager->getFontShader());
	m_pLoadingScene->setCamera(m_pCamera);

	GameManager::GetInstance()->ExecuteCommandList();
	GameManager::GetInstance()->ResetCommandList();

	GameManager::GetInstance()->SetLoadingScene(m_pLoadingScene);
	m_pScene = shared_ptr<StartScene>(new StartScene());
	m_pScene->SetGraphicsRootSignature(GameManager::GetInstance()->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates,m_ppd3dPipelineStates);
	m_pScene->BuildObjects();
	m_pScene->SetFontShader(m_pFontManager->getFontShader());
	m_pScene->setCamera(m_pCamera);
	
	//-----------------------
	
	GameManager::GetInstance()->ExecuteCommandList();

	if (m_pScene)
		m_pScene->ReleaseUploadBuffers();
	if (m_pLoadingScene)
		m_pLoadingScene->ReleaseUploadBuffers();

	m_pFontManager->ReleaseUploadBuffers();
	if (m_pPlayer)
		m_pPlayer->ReleaseUploadBuffers();

	NetWorkManager::GetInstance()->SetCurScene(m_pScene);
	EventHandler::GetInstance()->SetCurScene(m_pScene);
	m_GameTimer.Reset();
}

void CGameFramework::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((m_GameTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;     //1000ms-> 1초동안의 이 함수의 호출 수를 체크함.

		wstring fpsStr = to_wstring(fps);
		wstring mspfStr = to_wstring(mspf);
		::GetCursorPos(&m_ptOldCursorPos);

		ScreenToClient(m_hWnd, &m_ptOldCursorPos);
		Point2D p = ScreenToProj(GameManager::GetInstance()->GetWindowWidth(), GameManager::GetInstance()->GetWindowHeight(), m_ptOldCursorPos);
		wstring windowText;
	
		windowText = L"DinoRun   fps: " + fpsStr;
		
		

		SetWindowText(m_hWnd, windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
LRESULT CALLBACK CGameFramework::OnProcessingPacket(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SOCKET:
		if (WSAGETSELECTERROR(lParam))
		{
			//Network::GetInstance()->DeleteInstance();
			closesocket((SOCKET)wParam);
			PostQuitMessage(0);
		}
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			NetWorkManager::GetInstance()->ReadPacket(m_GameTimer.DeltaTime());
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			NetWorkManager::GetInstance()->Initialize();
			break;
		}
		break;
	}
	return(0);
}
LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		if (LOWORD(lParam) == 0 || HIWORD(lParam) == 0)
			break;
		GameManager::GetInstance()->ResetSize(LOWORD(lParam), HIWORD(lParam));

		EventHandler::GetInstance()->m_nWndClientWidth = LOWORD(lParam);
		EventHandler::GetInstance()->m_nWndClientHeight = HIWORD(lParam);

		m_pCamera->SetViewport(0, 0, LOWORD(lParam), HIWORD(lParam), 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, LOWORD(lParam), HIWORD(lParam));


		if (m_pScene)
		{
			m_pScene->ReSize();
			
		}
		if (m_pPlayer)
		{
			CCamera* camera = m_pPlayer->GetCamera();
			camera->SetViewport(0, 0, LOWORD(lParam), HIWORD(lParam), 0.0f, 1.0f);
			camera->SetScissorRect(0, 0, LOWORD(lParam), HIWORD(lParam));
			camera->ReGenerateProjectionMatrix((float)LOWORD(lParam) / (float)HIWORD(lParam));
		}
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam, m_GameTimer.DeltaTime());
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam, m_GameTimer.DeltaTime());
		break;
	case WM_DESTROY:
		break;

	}
	return(0);
}


////////////////////////////////////////////////////////////////////////
// 내부 함수
void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		m_pScene->ReleaseShaderVariables();
		m_pScene.reset();
		m_pScene = NULL;

		EventHandler::GetInstance()->ResetCurScene();
	}

	if (m_pPlayer)
	{
		m_pPlayer->Release();
		m_pPlayer = NULL;
	}

}

void CGameFramework::ChangeSceneByType(SceneType type)
{
	GameManager::GetInstance()->ResetCommandList();
	
	if (m_PrevState == Start_Scene)
	{
		//씬전환 시 이전 상태가 스타트인 경우 닉네임을 멤버에 저장해야함.
		m_sPlayerID = m_pScene->GetId(); 
	}
	ReleaseObjects();

	// 게임씬에 들어가는 액션에서는 기존의 player를 메모리 반납 후 캐릭터 번호에 맞는 모델로 새로 생성할것
	//이때 반납전에 아이디와 모델타입을 이곳에 만든 지역변수에 저장후 반납할 것.
	switch (type)
	{
	case Default_Scene:
		::PostQuitMessage(0);
		return;
		break;
	case Start_Scene:
		if (m_PrevState == SceneType::Room_Scene)
		{
			m_sPlayerID = "";
			m_pScene = shared_ptr<StartScene>(new StartScene());
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
		break;
	case Room_Scene:
		if (m_PrevState == SceneType::End_Scene || m_PrevState == SceneType::Start_Scene)
		{
			m_pScene = shared_ptr<RoomScene>(new RoomScene());
			m_pScene->SetId(m_sPlayerID);
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
		break;
	case Game_Scene:
		if (m_PrevState == SceneType::Room_Scene)
		{
			m_pScene = shared_ptr<GameScene>(new GameScene());
		}
		break;
	case ItemGame_Scene:
		if (m_PrevState == SceneType::Room_Scene)
		{
			m_pScene = shared_ptr<ItemGameScene>(new ItemGameScene());
		}
		break;
	case End_Scene:
		if (m_PrevState == SceneType::Game_Scene || m_PrevState == SceneType::ItemGame_Scene)
		{
			m_pScene = shared_ptr<EndScene>(new EndScene());
			m_pScene->SetId(m_sPlayerID);
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
		break;
	default:
		break;
	}

	m_pScene->SetGraphicsRootSignature(GameManager::GetInstance()->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates, m_ppd3dPipelineStates);
	m_pScene->BuildObjects();

	if (type == SceneType::Game_Scene || type == SceneType::ItemGame_Scene)
	{
		m_pScene->SetId(m_sPlayerID);
		m_pScene->SetFontShader(m_pFontManager->getFontShader());
		CDinoRunPlayer *pPlayer = new CDinoRunPlayer("Resources/Models/M_DinoTest.bin");
		pPlayer->SetMaxForce(MAX_FORCE);
		m_pPlayer = pPlayer;
		//게임씬으로 바뀌면서 자신의 위치정보를 서버로부터 받아서 설정한다.
#ifdef isConnectedToServer
		m_pPlayer->SetPosition(NetWorkManager::GetInstance()->GetPosition());
#endif
		if (type == SceneType::ItemGame_Scene)
			m_pPlayer->SetMaxVelocityXZ(25);

		m_pScene->setPlayer(m_pPlayer);
		m_pScene->setCamera(m_pPlayer->GetCamera());
	}

	GameManager::GetInstance()->ExecuteCommandList();

	if (m_pScene)
		m_pScene->ReleaseUploadBuffers();
	if (m_pPlayer)
		m_pPlayer->ReleaseUploadBuffers();

	m_PrevState = type;
	NetWorkManager::GetInstance()->SetCurScene(m_pScene);
	EventHandler::GetInstance()->SetCurScene(m_pScene);
}


void CGameFramework::BuildPipelineState()
{
	m_nPipelineStates = 30;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppd3dPipelineStates[i] = NULL;
	}
	CreatePSOs();
}
void CGameFramework::CreatePSOs()
{
	//Base Pipelines
	CreatePsoCube(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_CUBE_MAP);
	CreatePsoSkinMesh(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SKIN_MESH);
	CreatePsoTextedInstancing(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_MODEL_INSTANCING);
	CreatePsoBillBoardInstancing(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_BILLBOARD);
	CreatePsoTerrain(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_TERRAIN);
	CreatePsoSkinedInstancing(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SKIN_MESH_INSTANCING);
	CreatePsoUi(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_UI);
	CreatePsoUiGuage(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_UI_GAUGE);
	CreatePsoUiNumber(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_UI_NUMBER);
	CreatePsoParticle(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_PARTICLE);
	CreatePsoMinimap(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_MINIMAP);
	CreatePsoPostEffect(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_EFFECT);
	//Shadow Pipelines

	CreatePsoShadowSkinMesh(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_SKIN_MESH);//수정
	CreatePsoShadowTextedInstancing(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_MODEL_INSTANCING);
	CreatePsoShadowBillBoardInstancing(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_BILLBOARD);
	CreatePsoShadowTerrain(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_TERRAIN);
	CreatePsoShadowSkinedInstancing(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_SKINED_INSTANCING);
	
	//Velocity Pipelines
	CreatePsoVelocitySkinMesh(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_SKIN_MESH);//수정
	CreatePsoVelocityTextedInstancing(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_MODEL_INSTANCING);
	CreatePsoVelocityBillBoardInstancing(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_BILLBOARD);
	CreatePsoVelocityTerrain(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_TERRAIN);
	CreatePsoVelocitySkinedInstancing(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_SKINED_INSTANCING);
	CreatePsoVelocityCubeMap(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_CUBEMAP);
	//Wire Pipelines
	CreatePsoWire(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_WIRE);
	CreatePsoWireInstance(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_WIRE_INSTANCING);
	//Font
	CreatePsoFont(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_FONT);
	//Blur
	CreatePsoMotionBlur(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, PSO_MOTION_BLUR);

	CreatePsoParticleCs(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, PSO_PARTICLE_CALC);
}