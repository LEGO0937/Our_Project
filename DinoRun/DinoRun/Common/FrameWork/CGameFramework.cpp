#include "../stdafx.h"
#include "../Common/FrameWork/CGameFramework.h"
#include "TerrainObject.h"
#include "EventHandler/EventHandler.h"
ID3D12PipelineState** CGameFramework::m_ppd3dPipelineStates = NULL;


#ifdef _WITH_SERVER_
char CGameFramework::m_HostID = -1;
#endif

bool g_IsRoundEnd = false;



CGameFramework::CGameFramework()
{
	EventHandler::GetInstance()->Update();
}

CGameFramework::~CGameFramework()
{
	EventHandler::GetInstance()->destroy();
}

bool CGameFramework::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hWnd = hWnd;

	m_pCreateManager = shared_ptr<CreateManager>(new CreateManager);

	m_pCreateManager->Initialize(hInstance, hWnd);
	m_pDrawManager = m_pCreateManager->GetDrawMgr();

	::gnCbvSrvDescriptorIncrementSize = m_pCreateManager->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_pCamera = new CMinimapCamera;
	m_pCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pCamera->GenerateOrthoProjectionMatrix(1000, 1000, 10, 300.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	m_pCamera->GenerateViewMatrix(XMFLOAT3(0, 0, 0), XMFLOAT3(128 * TerrainScaleX, 0, 128 * TerrainScaleZ), XMFLOAT3(0, 0, 1));
	m_pCamera->CreateShaderVariables(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetCommandList().Get());


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
	m_pCreateManager->Release();
}

void LoadingFrameAdvance()
{

}
void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick();
	float fTimeElapsed = m_GameTimer.DeltaTime();

	m_pScene->ProcessInput(m_hWnd, fTimeElapsed);
	// processinput과 플레이어 animate의 순서를 뒤바꾸면 플레이어가 움직일 시 흔들림 발생 왜?
	m_pScene->FixedUpdate(m_pCreateManager.get(), fTimeElapsed);
	m_pScene->AnimateObjects(fTimeElapsed); //바뀐 행렬값으로 애니메이션 수행

	m_pCreateManager->GetDrawMgr()->WaitForGpuComplete();
	m_pCreateManager->GetDrawMgr()->ResetCommandAllocator();
	m_pCreateManager->ResetCommandList();
	m_pCreateManager->SetComputeRootSignature();

	m_CurState = m_pScene->Update(m_pCreateManager.get(), fTimeElapsed);  //ProcessInput과 Update를 통해 물리처리
	m_pCreateManager->ExecuteCommandList();
	//m_pCreateManager->GetDrawMgr()->WaitForGpuComplete();
	//fixed에서 충돌처리도 하게하여 오브젝트도 생성하게 하고 update에서 releaseUploadBuffer하는건 어떨까
	if (m_pScene)
		m_pDrawManager->Render(m_pScene, fTimeElapsed);

	if (m_CurState != m_PrevState)
	{
		ChangeSceneByType(m_CurState);
		//씬 교환 부분.
	}
}



void CGameFramework::BuildObjects()
{
	//m_pLoadingScene = shared_ptr<CLoadingScene>(new CLoadingScene());
	//m_pLoadingScene->Initialize(m_pCreateMgr, m_pNetwork);
	//m_pLoadingScene->ReleaseUploadBuffers();
	//m_pDrawMgr->SetLoadingScene(m_pLoadingScene);

	BuildPipelineState();

	//m_pCreateMgr->GetDrawMgr()->WaitForGpuComplete();
	m_pCreateManager->ResetCommandList();

	m_pFontManager = shared_ptr<FontManager>(new FontManager);
	m_pFontManager->Initialize(m_pCreateManager.get());
	//-----------
	// 인게임 작업용
	m_pLoadingScene = shared_ptr<LoadingScene>(new LoadingScene());
	m_pLoadingScene->SetGraphicsRootSignature(m_pCreateManager->GetGraphicsRootSignature().Get());
	m_pLoadingScene->SetPipelineStates(m_nPipelineStates, m_ppd3dPipelineStates);
	m_pLoadingScene->BuildObjects(m_pCreateManager);
	m_pLoadingScene->SetFontShader(m_pFontManager->getFontShader());
	m_pLoadingScene->setCamera(m_pCamera);
	m_pCreateManager->ExecuteCommandList();
	m_pCreateManager->ResetCommandList();

	m_pCreateManager->SetLoadingScene(m_pLoadingScene);

	m_pScene = shared_ptr<ItemGameScene>(new ItemGameScene());
	m_pScene->SetGraphicsRootSignature(m_pCreateManager->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates, m_ppd3dPipelineStates);
	m_pScene->BuildObjects(m_pCreateManager);

	CDinoRunPlayer* pPlayer = new CDinoRunPlayer(m_pCreateManager.get(), "Resources/Models/M_Dino.bin");
	pPlayer->SetMaxForce(MIN_FORCE);
	m_pPlayer = pPlayer;

	m_pScene->setPlayer(m_pPlayer);
	m_pScene->setCamera(m_pPlayer->GetCamera());

	//--------
	// 일반 로비씬 부터
	/*m_pLoadingScene = shared_ptr<LoadingScene>(new LoadingScene());
	m_pLoadingScene->SetGraphicsRootSignature(m_pCreateManager->GetGraphicsRootSignature().Get());
	m_pLoadingScene->SetPipelineStates(m_nPipelineStates, m_ppd3dPipelineStates);
	m_pLoadingScene->BuildObjects(m_pCreateManager);
	m_pLoadingScene->SetFontShader(m_pFontManager->getFontShader());
	m_pLoadingScene->setCamera(m_pCamera);
	m_pCreateManager->ExecuteCommandList();
	m_pCreateManager->ResetCommandList();

	m_pCreateManager->SetLoadingScene(m_pLoadingScene);
	m_pScene = shared_ptr<StartScene>(new StartScene());
	m_pScene->SetGraphicsRootSignature(m_pCreateManager->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates,m_ppd3dPipelineStates);
	m_pScene->BuildObjects(m_pCreateManager);
	m_pScene->SetFontShader(m_pFontManager->getFontShader());
	m_pScene->setCamera(m_pCamera);*/

	//-----------------------

	m_pCreateManager->ExecuteCommandList();

	if (m_pScene)
		m_pScene->ReleaseUploadBuffers();
	if (m_pLoadingScene)
		m_pLoadingScene->ReleaseUploadBuffers();

	m_pFontManager->ReleaseUploadBuffers();
	if (m_pPlayer)
		m_pPlayer->ReleaseUploadBuffers();

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
		Point2D p = ScreenToProj(m_pCreateManager->GetWindowWidth(), m_pCreateManager->GetWindowHeight(), m_ptOldCursorPos);
		wstring windowText;
		if (m_pPlayer)
		{
			windowText = L"DinoRun   fps: " + fpsStr + L"x:" + to_wstring(p.x) + L"   y:" + to_wstring(p.y)
				+ L"pX: " + to_wstring(m_pPlayer->GetPosition().x) + L"  z:" + to_wstring(m_pPlayer->GetPosition().z);
		}
		else
		{
			windowText = L"DinoRun   fps: " + fpsStr + L"x:" + to_wstring(p.x) + L"   y:" + to_wstring(p.y);
		}

		/*+ L"  x:" +
		to_wstring(m_pPlayer->m_xmf4x4World._41)
		+ L"  y:" + to_wstring(m_pPlayer->m_xmf4x4World._42)
		+ L"  z:" + to_wstring(m_pPlayer->m_xmf4x4World._43)
		+ L"  terrain x:  " + to_wstring((m_pScene->GetTerrain())->GetNormal(m_pPlayer->m_xmf4x4World._41, m_pPlayer->m_xmf4x4World._43).x)
		+ L"  terrain y:  " + to_wstring((m_pScene->GetTerrain())->GetNormal(m_pPlayer->m_xmf4x4World._41, m_pPlayer->m_xmf4x4World._43).y)
		+ L"  terrain z:  " + to_wstring((m_pScene->GetTerrain())->GetNormal(m_pPlayer->m_xmf4x4World._41, m_pPlayer->m_xmf4x4World._43).z);
	*/
	/*L"   mspf: " + mspfStr*/

		SetWindowText(m_hWnd, windowText.c_str());
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		if (LOWORD(lParam) == 0 || HIWORD(lParam) == 0)
			break;
		m_pCreateManager->Resize(LOWORD(lParam), HIWORD(lParam));

		m_pCamera->SetViewport(0, 0, LOWORD(lParam), HIWORD(lParam), 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, LOWORD(lParam), HIWORD(lParam));

		if (m_pScene)
		{
			m_pScene->ResetShadowBuffer(m_pCreateManager.get());
			m_pScene->SetWindowSize(m_pCreateManager->GetWindowWidth(), m_pCreateManager->GetWindowHeight());
		}
		if (m_pPlayer)
		{
			CCamera* camera = m_pPlayer->GetCamera();
			camera->GenerateProjectionMatrix(1.01f, 5000.0f, LOWORD(lParam) / HIWORD(lParam), 60.0f);
			camera->SetViewport(0, 0, LOWORD(lParam), HIWORD(lParam), 0.0f, 1.0f);
			camera->SetScissorRect(0, 0, LOWORD(lParam), HIWORD(lParam));
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
	//if (m_pLoadingScene) m_pLoadingScene->Release();
}

void CGameFramework::ChangeSceneByType(SceneType type)
{
	m_pCreateManager->ResetCommandList();


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
		if (m_PrevState == SceneType::Lobby_Scene)
		{
			m_sPlayerID = "Abcd";
			m_pScene = shared_ptr<StartScene>(new StartScene());
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
		break;
	case Lobby_Scene:
		if (m_PrevState == SceneType::Start_Scene || m_PrevState == SceneType::Room_Scene)
		{
			m_pScene = shared_ptr<LobbyScene>(new LobbyScene());
			m_pScene->SetId(m_sPlayerID);
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
		break;
	case Room_Scene:
		if (m_PrevState == SceneType::Lobby_Scene || m_PrevState == SceneType::End_Scene)
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

	m_pScene->SetGraphicsRootSignature(m_pCreateManager->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates, m_ppd3dPipelineStates);
	m_pScene->BuildObjects(m_pCreateManager);

	if (type == SceneType::Game_Scene || type == SceneType::ItemGame_Scene)
	{
		m_pScene->SetId(m_sPlayerID);
		CDinoRunPlayer* pPlayer = new CDinoRunPlayer(m_pCreateManager.get(), "Resources/Models/M_Dino.bin");
		pPlayer->SetMaxForce(MAX_FORCE);
		m_pPlayer = pPlayer;

		m_pScene->setPlayer(m_pPlayer);
		m_pScene->setCamera(m_pPlayer->GetCamera());


	}

	m_pCreateManager->ExecuteCommandList();

	if (m_pScene)
		m_pScene->ReleaseUploadBuffers();
	if (m_pPlayer)
		m_pPlayer->ReleaseUploadBuffers();

	m_PrevState = type;

	EventHandler::GetInstance()->SetCurScene(m_pScene);
}


void CGameFramework::BuildPipelineState()
{
	m_nPipelineStates = 29;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];
	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppd3dPipelineStates[i] = NULL;
	}
	CreatePSOs();
}
void CGameFramework::CreatePSOs()
{
	//Base Pipelines
	CreatePsoCube(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_CUBE_MAP);
	CreatePsoSkinMesh(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SKIN_MESH);
	CreatePsoTextedInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_MODEL_INSTANCING);
	CreatePsoBillBoardInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_BILLBOARD);
	CreatePsoTerrain(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_TERRAIN);
	CreatePsoSkinedInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SKIN_MESH_INSTANCING);
	CreatePsoUi(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_UI);
	CreatePsoUiGuage(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_UI_GAUGE);
	CreatePsoUiNumber(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_UI_NUMBER);
	CreatePsoParticle(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_PARTICLE);
	CreatePsoMinimap(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_MINIMAP);
	CreatePsoPostEffect(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_EFFECT);
	//Shadow Pipelines

	CreatePsoShadowSkinMesh(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_SKIN_MESH);//수정
	CreatePsoShadowTextedInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_MODEL_INSTANCING);
	CreatePsoShadowBillBoardInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_BILLBOARD);
	CreatePsoShadowTerrain(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_TERRAIN);
	CreatePsoShadowSkinedInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_SKINED_INSTANCING);

	//Velocity Pipelines
	CreatePsoVelocitySkinMesh(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_SKIN_MESH);//수정
	CreatePsoVelocityTextedInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_MODEL_INSTANCING);
	CreatePsoVelocityBillBoardInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_BILLBOARD);
	CreatePsoVelocityTerrain(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_TERRAIN);
	CreatePsoVelocitySkinedInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_VELOCITY_SKINED_INSTANCING);

	//Wire Pipelines
	CreatePsoWire(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_WIRE);
	CreatePsoWireInstance(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_WIRE_INSTANCING);
	//Font
	CreatePsoFont(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_PONT);
	//Blur
	CreatePsoMotionBlur(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, PSO_MOTION_BLUR);
	CreatePsoHorzBlur(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, PSO_HORZ_BLUR);
	CreatePsoVertBlur(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, PSO_VERT_BLUR);

	CreatePsoParticleCs(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, PSO_PARTICLE_CALC);
}

// 프로세스 패킷 수정 필요 
void CGameFramework::ProcessPacket(char* packet)
{
	switch (packet[1])
	{
	case SC_ACCESS_COMPLETE:
	{
		SC_PACKET_ACCESS_COMPLETE* pAC = reinterpret_cast<SC_PACKET_ACCESS_COMPLETE*>(packet);
		//플레이어 아이디 Set
		m_pPlayer->SetPlayerID(pAC->myId);
		NetWorkManager::GetInstance()->SetMyID(pAC->myId);
		m_HostID = pAC->hostId;

		//reinterpret_cast<CCharacterSelectUIShader*>(m_pLobbyScene->GetCharacterSelectShader())->ChangeHost();

		CCharacterSelectUIShader::SetMyID(pAC->myId);

		//cout << "0. 접속 완료!! ▶ ";
		//cout << "MyID : " << (int)m_pPlayer->GetPlayerID() << ", HostID : " << m_HostID << endl;
		break;
	}

	case SC_ACCESS_PLAYER:
	{
		SC_PACKET_ACCESS_PLAYER* pAP = reinterpret_cast<SC_PACKET_ACCESS_PLAYER*>(packet);
		//cout << "1. OtherPlayer 접속!! ▶";
		//cout << "OtherPlayerID : " <<(int) pAP->id << endl;
		break;
	}


	case SC_CLIENT_LOBBY_IN:
	{
		SC_PACKET_LOBBY_IN* pLI = reinterpret_cast<SC_PACKET_LOBBY_IN*>(packet);

		/*m_mapClients[(int)pLI->id] = pLI->client_state;
		m_mapClients[(int)pLI->id].isReady = pLI->client_state.isReady;
		m_mapClients[(int)pLI->id].id = pLI->id;*/

		//맵의 emplace는 한번 생성하면 똑같은 키에 value를 넣는 작업을 하지 않는다.(중복을 허용하지 않기 때문에)
		string user = m_mapClients[(int)pLI->id].name;
		string s = "님이 입장하였습니다.";

		/*ChattingSystem::GetInstance()->PushChattingText(user, s.c_str());*/

		//cout << "3. 로비에 접속" << endl;
		//cout << "m_mapClients 사이즈" << m_mapClients.size() << endl;
		break;
	}


	case SC_CLIENT_LOBBY_OUT:
	{
		printf("SC_CLIENT_LOBBY_OUT 호출");
		SC_PACKET_LOBBY_OUT* pLO = reinterpret_cast<SC_PACKET_LOBBY_OUT*>(packet);

		if (pLO->id < MAX_USER)
		{
			//char id = m_mapClients[pLO->id].id;
			//// 나갔으면 캐릭터 초기화
			//CLobbyScene::AddClientsCharacter(pLO->id, -1);

			m_mapClients[(int)pLO->id].isReady = false;
			string user = m_mapClients[(int)pLO->id].name;
			string s = "님이 나갔습니다.";

			ChattingSystem::GetInstance()->PushChattingText(user, s.c_str());
			strcpy(m_mapClients[(int)pLO->id].name, " ");

			auto mapIter = find_if(m_mapClients.begin(), m_mapClients.end(), [&id](const pair<char, clientsInfo>& p)
			{
				return p.first == id;
			});
			if (mapIter != m_mapClients.end())
			{
				m_mapClients.erase(mapIter);
			}

			//cout << "m_mapClients 사이즈" << m_mapClients.size() << endl;
		}
		break;
	}

	case SC_CHANGE_HOST_ID:
	{
		SC_PACKET_CHANGE_HOST* pCH = reinterpret_cast<SC_PACKET_CHANGE_HOST*>(packet);

		if (pCH->hostID < MAX_USER)
		{
			m_HostID = pCH->hostID;

			reinterpret_cast<CCharacterSelectUIShader*>(m_pLobbyScene->GetCharacterSelectShader())->ChangeHost();

			// 모든 클라의 레디를 해제함
			for (auto& client : m_mapClients)
				client.second.isReady = false;
		}

		break;
	}

	case SC_READY_STATE:
	{
		SC_PACKET_READY_STATE* pReady = reinterpret_cast<SC_PACKET_READY_STATE*>(packet);

		m_mapClients[pReady->id].isReady = true;

		//cout << "SC_READY_STATE 호출" << endl;
		break;
	}

	case SC_UNREADY_STATE:
	{
		SC_PACKET_UNREADY_STATE* pNotReady = reinterpret_cast<SC_PACKET_UNREADY_STATE*>(packet);

		m_mapClients[pNotReady->id].isReady = false;
		break;
	}

	case SC_CHATTING:
	{
		SC_PACKET_CHATTING* pCh = reinterpret_cast<SC_PACKET_CHATTING*>(packet);

		const string& clientName = m_mapClients[pCh->id].name;

		ChattingSystem::GetInstance()->ResetShowTime(0.0f);
		//ChattingSystem::GetInstance()->SetActive(true);
		ChattingSystem::GetInstance()->PushChattingText(clientName, pCh->message);
		break;
	}

	case SC_PLEASE_READY:
	{
		Network::GetInstance()->SetNullRS();

		string str = "모든 플레이어가 Ready하지 않았습니다.레디해주세요.";
		ChattingSystem::GetInstance()->PushText(str);
		//printf("모든 플레이어가 Ready하지 않았습니다.\n");
		break;
	}

	case SC_ROUND_START:
	{




		break;
	}

	case SC_PUT_PLAYER:
	{
		SC_PACKET_PUT_PLAYER* pPP = reinterpret_cast<SC_PACKET_PUT_PLAYER*>(packet);
		// 앞으로 Put Player는 라운드 시작 시 캐릭터들의 시작 위치를 받는 패킷으로 사용



		XMFLOAT3 pos = CMapObjectsShader::spawn[g_Round][pPP->posIdx[m_pPlayer->GetPlayerID()]].pos;

		// 초기 플레이어 위치
		m_pPlayer->SetPosition(pos);
		m_pPlayer->SetLookVector(XMFLOAT3(0.0f, 0.0f, 1.0f));
		m_pPlayer->SetUpVector(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_pPlayer->SetRightVector(XMFLOAT3(1.0f, 0.0f, 0.0f));

		//모든 아이템 보유 초기화
		m_pPlayer->SetIsRock(false);
		m_pPlayer->SetIsBanana(false);
		m_pPlayer->SetIsMud(false);

		auto iter = m_pScene->getShaderManager()->getShaderMap().find("OtherPlayer");
		if (iter != m_pScene->getShaderManager()->getShaderMap().end())
		{
			for (auto enemy : m_mapClients)
			{
				if (enemy.second.id == m_pPlayer->GetPlayerID())   continue;

				XMFLOAT3 pos = CMapObjectsShader::spawn[g_Round][pPP->posIdx[enemy.second.id]].pos;
				(*iter).second->m_ppObjects[enemy.second.id]->SetPosition(pos);
				(*iter).second->m_ppObjects[enemy.second.id]->SetLookVector(XMFLOAT3(0.0f, 0.0f, 1.0f));
				(*iter).second->m_ppObjects[enemy.second.id]->SetRightVector(XMFLOAT3(1.0f, 0.0f, 0.0f));
				(*iter).second->m_ppObjects[enemy.second.id]->SetUpVector(XMFLOAT3(0.0f, 1.0f, 0.0f));
				(*iter).second->m_ppObjects[enemy.second.id]->SetScale(10, 10, 10);
				//모든 아이템 보유 초기화
				(*iter).second->m_ppObjects[enemy.second.id]->SetIsRock(false);
				(*iter).second->m_ppObjects[enemy.second.id]->SetIsBanana(false);
				(*iter).second->m_ppObjects[enemy.second.id]->SetIsMud(false);
			}
		}
		break;
	}

	case SC_MOVE_PLAYER:
	{
		SC_PACKET_MOVE_PLAYER* pMP = reinterpret_cast<SC_PACKET_MOVE_PLAYER*>(packet);
		//m_timer.Stop();
		//m_LatencyTime = m_timer.GetTotalTime();
		//cout << "지연시간 : "<< m_LatencyTime << endl;
		// 지연시간 초기화
		//m_LatencyTime = 0.f;

		if (pMP->id == m_pPlayer->GetPlayerID())
		{
			XMFLOAT3 pos = XMFLOAT3(pMP->xPos, pMP->yPos, pMP->zPos);
			XMFLOAT3 look = XMFLOAT3(pMP->xLook, pMP->yLook, pMP->zLook);
			XMFLOAT3 up = XMFLOAT3(pMP->xUp, pMP->yUp, pMP->zUp);
			XMFLOAT3 right = XMFLOAT3(pMP->xRight, pMP->yRight, pMP->zRight);

			//g_IsStop = true;
			//cout << "서버 위치 : " << pos.x << ", " << pos.y << ", " << pos.z << endl;
			//cout << "클라 위치 : " << m_pPlayer->GetPosition().x << ", " << m_pPlayer->GetPosition().y << ", " << m_pPlayer->GetPosition().z << endl;

			static float elapsedTime = 0.0f;
			if (elapsedTime > 5.0f || m_pPlayer->GetCollision() == true)
			{
				m_pPlayer->SetPosition(pos);
				elapsedTime = 0.0f;
			}
			else
			{
				elapsedTime += m_GameTimer.GetTimeElapsed();
			}

			m_pPlayer->SetLookVector(look);
			m_pPlayer->SetUpVector(up);
			m_pPlayer->SetRightVector(right);
			//m_pPlayer->SetCollision(false);
		}

		else if (pMP->id < MAX_USER)
		{
			char id = pMP->id;

			XMFLOAT3 pos = XMFLOAT3(pMP->xPos, pMP->yPos, pMP->zPos);
			XMFLOAT3 look = XMFLOAT3(pMP->xLook, pMP->yLook, pMP->zLook);
			XMFLOAT3 up = XMFLOAT3(pMP->xUp, pMP->yUp, pMP->zUp);
			XMFLOAT3 right = XMFLOAT3(pMP->xRight, pMP->yRight, pMP->zRight);

			auto iter = m_pScene->getShaderManager()->getShaderMap().find("OtherPlayer");
			if (iter != m_pScene->getShaderManager()->getShaderMap().end())
			{
				//char id = pMP->id;

				XMFLOAT3 pos = XMFLOAT3(pMP->xPos, pMP->yPos, pMP->zPos);
				XMFLOAT3 look = XMFLOAT3(pMP->xLook, pMP->yLook, pMP->zLook);
				XMFLOAT3 up = XMFLOAT3(pMP->xUp, pMP->yUp, pMP->zUp);
				XMFLOAT3 right = XMFLOAT3(pMP->xRight, pMP->yRight, pMP->zRight);

				auto iter = m_pScene->getShaderManager()->getShaderMap().find("OtherPlayer");
				if (iter != m_pScene->getShaderManager()->getShaderMap().end())
				{
					char enemyID = m_mapClients[pMP->id].id;

					(*iter).second->m_ppObjects[enemyID]->SetPosition(pos);
					(*iter).second->m_ppObjects[enemyID]->SetLookVector(look);
					(*iter).second->m_ppObjects[enemyID]->SetRightVector(right);
					(*iter).second->m_ppObjects[enemyID]->SetUpVector(up);
					(*iter).second->m_ppObjects[enemyID]->SetScale(10, 10, 10);
					(*iter).second->m_ppObjects[enemyID]->SetVelocityFromServer(pMP->fVelocity);
				}
			}
		}
		break;
	}

	case SC_ANIMATION_INFO:
	{

		break;
	}


	case SC_REMOVE_PLAYER:
	{
		SC_PACKET_REMOVE_PLAYER* pRP = reinterpret_cast<SC_PACKET_REMOVE_PLAYER*>(packet);
		m_HostID = pRP->hostId;
		m_pScene->RemovePlayer(pRP->id);

		if (pRP->id != m_pPlayer->GetPlayerID())
		{
			auto iter = m_pScene->getShaderManager()->getShaderMap().find("OtherPlayer");

			if (iter != m_pScene->getShaderManager()->getShaderMap().end())
			{
				char id = m_mapClients[pRP->id].id;
				//vector<pair<char, char>>& vec = dynamic_cast<CSkinnedAnimationObjectShader*>((*iter).second)->m_vMaterial;

				(*iter).second->m_ppObjects[id]->SetPosition(0.0f, 0.0f, 0.0f);

				string s = "님이 나갔습니다.";
				string user = m_mapClients[id].name;

				auto mapIter = find_if(m_mapClients.begin(), m_mapClients.end(), [&id](const pair<char, clientsInfo>& p)
				{
					return p.first == id;
				});
				if (mapIter != m_mapClients.end())
				{
					m_mapClients.erase(mapIter);
				}
				//채팅 창에서 보여지는 시간 reset
				ChattingSystem::GetInstance()->ResetShowTime(0.0f);
				ChattingSystem::GetInstance()->PushChattingText(user, s.c_str());
			}
		}


		printf("Player Disconnected ID : %d\n", pRP->id);
		break;
	}

	case SC_ROUND_END:
	{
		SC_PACKET_ROUND_END* pRE = reinterpret_cast<SC_PACKET_ROUND_END*>(packet);

		if (m_pPlayer != nullptr)
			m_pPlayer->ProcessRoundEnd();

		// 라운드가 끝났음을 표시

		m_RoundStartTime = std::chrono::steady_clock::now() + 10s;




		break;
	}

	case SC_COMPARE_TIME:
	{
		SC_PACKET_COMPARE_TIME* pCT = reinterpret_cast<SC_PACKET_COMPARE_TIME*>(packet);

		auto iter = m_pScene->getShaderManager()->getShaderMap().find("TimerUI");
		if (iter != m_pScene->getShaderManager()->getShaderMap().end())
			dynamic_cast<CTimerUIShader*>((*iter).second)->CompareServerTimeAndSet(pCT->serverTime);
		break;
	}



	case SC_GET_ITEM:
	{
		SC_PACKET_GET_ITEM* pGI = reinterpret_cast<SC_PACKET_GET_ITEM*>(packet);

		if (pGI->id == m_pPlayer->GetPlayerID() && m_pScene != nullptr)
		{
			string sItem = pGI->itemIndex;
			//cout << sItem<<"\n";


			m_pScene->MappingItemStringToItemType(sItem, itemType);

			m_pPlayer->Add_Inventory(sItem, itemType);

			switch (itemType)
			{
				// 여기다 락, 머드, 바나나 등 삽입
				// ex
				// case 아이템::아이템타입이 바난나
				//     player->SetBanana(true)
				//     break;
				// 나중에 할 작업 슈도코드
			}
		}



		//m_pPlayer->Add_Inventory(sItem,itemType);
	break;

	}

	

	case SC_USE_ITEM:
	{
		//SC_PACKET_USE_ITEM* pUI = reinterpret_cast<SC_PACKET_USE_ITEM*>(packet);

		//switch (pUI->usedItem)
		//{
		//case :
		//{
		//	
		//	break;
		//}
		//case 
		//{
		//


		//	break;
		//}
		//case
		//{
		//	
		//	break;
		//}
		//case 
		//{
		//	break;
		//}
		//default:
		//	//cout << "미정의 아이템\n";
		//	break;
		//}

		break;

	}


	case SC_GO_LOBBY:
	{
		SC_PACKET_GO_LOBBY* pGL = reinterpret_cast<SC_PACKET_GO_LOBBY*>(packet);

		for (auto& client : m_mapClients)
			client.second.isReady = false;
		m_pLobbyScene->LobbySceneClear();
		m_pScene->InGameSceneClear(m_pd3dDevice, m_pd3dCommandList);

		g_State = LOBBY;

		//cout << "Go Lobby" << endl;
		break;
	}
}



