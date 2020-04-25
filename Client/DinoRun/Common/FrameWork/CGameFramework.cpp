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

	m_pCreateMgr = shared_ptr<CreateManager>(new CreateManager);

	m_pCreateMgr->Initialize(hInstance, hWnd);
	m_pDrawMgr = m_pCreateMgr->GetDrawMgr();

	::gnCbvSrvDescriptorIncrementSize = m_pCreateMgr->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_pCamera = new CMinimapCamera;
	m_pCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pCamera->GenerateOrthoProjectionMatrix(1000, 1000, 10, 300.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	m_pCamera->GenerateViewMatrix(XMFLOAT3(0, 0, 0), XMFLOAT3(128 * TerrainScaleX, 0, 128 * TerrainScaleZ), XMFLOAT3(0, 0, 1));
	m_pCamera->CreateShaderVariables(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetCommandList().Get());
	
	
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
		m_pCreateMgr->Release();
}

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick();
	float fTimeElapsed = m_GameTimer.DeltaTime();
	if(m_CurState != m_PrevState)
	{
		ChangeSceneByType(m_CurState);
		//씬 교환 부분.
	}
	m_pScene->ProcessInput(m_hWnd, fTimeElapsed);
	// processinput과 플레이어 animate의 순서를 뒤바꾸면 플레이어가 움직일 시 흔들림 발생 왜?
	m_pScene->FixedUpdate(fTimeElapsed);
	m_pScene->AnimateObjects(fTimeElapsed); //바뀐 행렬값으로 애니메이션 수행
	m_CurState = m_pScene->Update(fTimeElapsed);  //ProcessInput과 Update를 통해 물리처리
	
	m_pDrawMgr->Render(m_pScene);
}

void CGameFramework::BuildObjects()
{
	//m_pLoadingScene = shared_ptr<CLoadingScene>(new CLoadingScene());
	//m_pLoadingScene->Initialize(m_pCreateMgr, m_pNetwork);
	//m_pLoadingScene->ReleaseUploadBuffers();
	//m_pDrawMgr->SetLoadingScene(m_pLoadingScene);
	
	BuildPipelineState();

	m_pCreateMgr->ResetCommandList();

	m_pFontManager = shared_ptr<FontManager>(new FontManager);
	m_pFontManager->Initialize(m_pCreateMgr);
	//-----------
	/*
	m_pScene = shared_ptr<GameScene>(new GameScene());
	m_pScene->SetGraphicsRootSignature(m_pCreateMgr->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates,m_ppd3dPipelineStates);
	m_pScene->BuildObjects(m_pCreateMgr);

	CDinoRunPlayer *pPlayer = new CDinoRunPlayer(m_pCreateMgr);
	m_pPlayer = pPlayer;

	m_pScene->setPlayer(m_pPlayer);
	m_pScene->setCamera(m_pPlayer->GetCamera());
	*/
	//--------
	
	m_pScene = shared_ptr<StartScene>(new StartScene());
	m_pScene->SetGraphicsRootSignature(m_pCreateMgr->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates,m_ppd3dPipelineStates);
	m_pScene->BuildObjects(m_pCreateMgr);
	m_pScene->SetFontShader(m_pFontManager->getFontShader());
	m_pScene->setCamera(m_pCamera);
	
	//-----------------------

	m_pCreateMgr->ExecuteCommandList();
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
		Point2D p = ScreenToProj(m_pCreateMgr->GetWindowWidth(), m_pCreateMgr->GetWindowHeight(), m_ptOldCursorPos);
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
		m_pCreateMgr->Resize(LOWORD(lParam), HIWORD(lParam));

		m_pCamera->SetViewport(0, 0, LOWORD(lParam), HIWORD(lParam), 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, LOWORD(lParam), HIWORD(lParam));

		if (m_pScene)
		{
			m_pScene->ResetShadowBuffer(m_pCreateMgr);
			m_pScene->SetWindowSize(m_pCreateMgr->GetWindowWidth(), m_pCreateMgr->GetWindowHeight());
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
	m_pCreateMgr->ResetCommandList();
	
	ReleaseObjects();

	// 게임씬에 들어가는 액션에서는 기존의 player를 메모리 반납 후 캐릭터 번호에 맞는 모델로 새로 생성할것
	//이때 반납전에 아이디와 모델타입을 이곳에 만든 지역변수에 저장후 반납할 것.
	if (type == SceneType::Start_Scene)
	{
		if (m_PrevState == SceneType::Lobby_Scene)
		{
			m_pScene = shared_ptr<StartScene>(new StartScene());
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
	}
	else if (type == SceneType::Lobby_Scene)
	{
		if(m_PrevState == SceneType::Start_Scene || m_PrevState == SceneType::Room_Scene)
		{ 
			m_pScene = shared_ptr<LobbyScene>(new LobbyScene());
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
	}
	else if (type == SceneType::Room_Scene)
	{
		if (m_PrevState == SceneType::Lobby_Scene || m_PrevState == SceneType::End_Scene)
		{
			m_pScene = shared_ptr<RoomScene>(new RoomScene());
			m_pScene->SetFontShader(m_pFontManager->getFontShader());
			m_pScene->setCamera(m_pCamera);
		}
	}
	else if (type == SceneType::Game_Scene)
	{
		if (m_PrevState == SceneType::Room_Scene)
		{
			m_pScene = shared_ptr<GameScene>(new GameScene());
		}
	}
	else if (type == SceneType::End_Scene)
	{
		if (m_PrevState == SceneType::Game_Scene)
		{
		}
	}

	m_pScene->SetGraphicsRootSignature(m_pCreateMgr->GetGraphicsRootSignature().Get());
	m_pScene->SetPipelineStates(m_nPipelineStates, m_ppd3dPipelineStates);
	m_pScene->BuildObjects(m_pCreateMgr);

	if (type == SceneType::Game_Scene)
	{
		CDinoRunPlayer *pPlayer = new CDinoRunPlayer(m_pCreateMgr);
		m_pPlayer = pPlayer;

		m_pScene->setPlayer(m_pPlayer);
		m_pScene->setCamera(m_pPlayer->GetCamera());


	}
	m_pCreateMgr->ExecuteCommandList();
	m_pScene->ReleaseUploadBuffers();
	if (m_pPlayer)
		m_pPlayer->ReleaseUploadBuffers();

	m_PrevState = type;
}


void CGameFramework::BuildPipelineState()
{
	m_nPipelineStates = 21;
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
	CreatePsoCube(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 0);
	CreatePsoSkinMesh(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 1);
	CreatePsoTextedInstancing(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 2);
	CreatePsoBillBoardInstancing(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 3);
	CreatePsoTerrain(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 4);
	CreatePsoSkinedInstancing(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 5);
	CreatePsoUi(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 16);
	CreatePsoUiGuage(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 13);
	CreatePsoUiNumber(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 14);
	CreatePsoParticle(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 19);
	//Shadow Pipelines

	CreatePsoShadowSkinMesh(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 6);//수정
	CreatePsoShadowTextedInstancing(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 7);
	CreatePsoShadowBillBoardInstancing(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 8);
	CreatePsoShadowTerrain(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 9);
	CreatePsoShadowSkinedInstancing(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 10);

	//Wire Pipelines
	CreatePsoWire(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 11);
	CreatePsoWireInstance(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 12);
	//Font
	CreatePsoFont(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetGraphicsRootSignature().Get(), m_ppd3dPipelineStates, 15);
	//Blur
	CreatePsoHorzBlur(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, 17);
	CreatePsoVertBlur(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, 18);

	CreatePsoParticleCs(m_pCreateMgr->GetDevice().Get(), m_pCreateMgr->GetComputeRootSignature().Get(), m_ppd3dPipelineStates, 20);
}