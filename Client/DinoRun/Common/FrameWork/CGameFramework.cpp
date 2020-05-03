#include "../stdafx.h"
#include "CGameFramework.h"
#include "../../Objects/TerrainObject.h"

ID3D12PipelineState** CGameFramework::m_ppd3dPipelineStates = NULL;

CGameFramework::CGameFramework()
{

}

CGameFramework::~CGameFramework()
{
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
	m_CurState = m_pScene->Update(m_pCreateManager.get(),fTimeElapsed);  //ProcessInput과 Update를 통해 물리처리
	m_pCreateManager->ExecuteCommandList();
	//m_pCreateManager->GetDrawMgr()->WaitForGpuComplete();
	//fixed에서 충돌처리도 하게하여 오브젝트도 생성하게 하고 update에서 releaseUploadBuffer하는건 어떨까
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
	/*
	m_pScene = shared_ptr<GameScene>(new GameScene());
	m_pScene->SetGraphicsRootSignature(m_pCreateMgr->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates,m_ppd3dPipelineStates);
	m_pScene->BuildObjects(m_pCreateMgr.get());

	CDinoRunPlayer *pPlayer = new CDinoRunPlayer(m_pCreateMgr.get());
	pPlayer->SetMaxForce(MIN_FORCE);
	m_pPlayer = pPlayer;

	m_pScene->setPlayer(m_pPlayer);
	m_pScene->setCamera(m_pPlayer->GetCamera());
	*/
	//--------
	
	m_pScene = shared_ptr<StartScene>(new StartScene());
	m_pScene->SetGraphicsRootSignature(m_pCreateManager->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates,m_ppd3dPipelineStates);
	m_pScene->BuildObjects(m_pCreateManager.get());
	m_pScene->SetFontShader(m_pFontManager->getFontShader());
	m_pScene->setCamera(m_pCamera);
	
	//-----------------------

	m_pCreateManager->ExecuteCommandList();

	if (m_pScene)
		m_pScene->ReleaseUploadBuffers();
	m_pFontManager->ReleaseUploadBuffers();
	if (m_pPlayer)
		m_pPlayer->ReleaseUploadBuffers();

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
		wstring windowText = L"DinoRun   fps: " + fpsStr + L"x:" + to_wstring(p.x)+L"   y:"+ to_wstring(p.y);
			
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
		//씬전환 시 이전 상태가 스타트인 경우 아이디와 비밀번호를 멤버에 저장해야함.
		m_sPlayerID = m_pScene->GetId();
		m_sPlayerPASSWORD = m_pScene->GetPassWord();
	}
	ReleaseObjects();

	// 게임씬에 들어가는 액션에서는 기존의 player를 메모리 반납 후 캐릭터 번호에 맞는 모델로 새로 생성할것
	//이때 반납전에 아이디와 모델타입을 이곳에 만든 지역변수에 저장후 반납할 것.
	switch (type)
	{
	case Default_Scene:
		break;
	case Start_Scene:
		if (m_PrevState == SceneType::Lobby_Scene)
		{
			m_sPlayerID = "";
			m_sPlayerPASSWORD = "";
			m_pScene = shared_ptr<StartScene>(new StartScene());
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
		break;
	case Lobby_Scene:
		if (m_PrevState == SceneType::Start_Scene || m_PrevState == SceneType::Room_Scene)
		{
			m_pScene = shared_ptr<LobbyScene>(new LobbyScene());
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
		break;
	case Room_Scene:
		if (m_PrevState == SceneType::Lobby_Scene || m_PrevState == SceneType::End_Scene)
		{
			m_pScene = shared_ptr<RoomScene>(new RoomScene());
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
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
		break;
	default:
		break;
	}

	m_pScene->SetGraphicsRootSignature(m_pCreateManager->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates, m_ppd3dPipelineStates);
	m_pScene->BuildObjects(m_pCreateManager.get());

	if (type == SceneType::Game_Scene || type == SceneType::ItemGame_Scene)
	{
		CDinoRunPlayer *pPlayer = new CDinoRunPlayer(m_pCreateManager.get());
		pPlayer->SetMaxForce(MIN_FORCE);
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
}


void CGameFramework::BuildPipelineState()
{
	m_nPipelineStates = 22;
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
	//Shadow Pipelines

	CreatePsoShadowSkinMesh(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_SKIN_MESH);//수정
	CreatePsoShadowTextedInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_MODEL_INSTANCING);
	CreatePsoShadowBillBoardInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_BILLBOARD);
	CreatePsoShadowTerrain(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_TERRAIN);
	CreatePsoShadowSkinedInstancing(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_SHADOW_SKINED_INSTANCING);

	//Wire Pipelines
	CreatePsoWire(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_WIRE);
	CreatePsoWireInstance(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_WIRE_INSTANCING);
	//Font
	CreatePsoFont(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, PSO_PONT);
	//Blur
	CreatePsoHorzBlur(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, PSO_HORZ_BLUR);
	CreatePsoVertBlur(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, PSO_VERT_BLUR);

	CreatePsoParticleCs(m_pCreateManager->GetDevice().Get(), m_pCreateManager->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, PSO_PARTICLE_CALC);
}