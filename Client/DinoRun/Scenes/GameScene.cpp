#include "GameScene.h"
#include "../Common/FrameWork/CreateManager.h"
#include "../Common/FrameWork/NetWorkManager.h"
#include "../Common/FrameWork/SoundManager.h"

#include "../Objects/PlayerObject.h"
#include "../Objects/SkyBoxObject.h"
#include "../Objects/TerrainObject.h"


#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/ModelShader/ModelShader.h"
#include "../CShaders/SkinedShader/SkinedShader.h"
#include "../CShaders/BlurShader/BlurShader.h"
#include "../CShaders/MotionBlurShader/MotionBlurShader.h"
#include "../CShaders/MinimapShader/MinimapShader.h"

#include "../Common/ParticleSystem/ParticleSystem.h"

#include "../CShaders/UiShader/UiShader.h"


#include "../Common/Camera/Camera.h"
#include "EventHandler/EventHandler.h"


GameScene::GameScene() :BaseScene()
{
	sceneType = SceneType::Game_Scene;
}
GameScene::~GameScene()
{
	m_pSoundManager->Stop("InGame_BGM");
}
void GameScene::ReleaseUploadBuffers()
{
	BaseScene::ReleaseUploadBuffers();
	if (m_pTerrain)
		m_pTerrain->ReleaseUploadBuffers();
	if (m_pSkyBox)
		m_pSkyBox->ReleaseUploadBuffers();
	if (m_pCheckPointShader)
		m_pCheckPointShader->ReleaseUploadBuffers();

	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) shader->ReleaseUploadBuffers();
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) shader->ReleaseUploadBuffers();
	for (CSkinedObInstancingShader* shader : instancingAnimatedModelShaders)
		if (shader) shader->ReleaseUploadBuffers();
	for (CUiShader* shader : instancingNumberUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
	for(CUiShader* shader : instancingImageUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }

	if (m_pMinimapShader)
		m_pMinimapShader->ReleaseUploadBuffers();
	if (m_pIconShader)
		m_pIconShader->ReleaseUploadBuffers();
	if (m_pGuageShader)
		m_pGuageShader->ReleaseUploadBuffers();
	if (m_pEffectShader)
		m_pEffectShader->ReleaseUploadBuffers();
}
void GameScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();
	if (m_pTerrain)
		m_pTerrain->Release();

	for (ParticleSystem* system : particleSystems)
	{
		system->Release();
	}
	if (m_pSkyBox)
	{
		m_pSkyBox->Release();
		m_pSkyBox = NULL;
	}

	if (m_pCheckPointShader)
	{
		m_pCheckPointShader->ReleaseShaderVariables();
		m_pCheckPointShader->ReleaseObjects();
		m_pCheckPointShader->Release();
	}
	for (CObjectsShader* shader : UpdatedShaders)
		if (shader) { shader->Release(); }

	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CSkinedObInstancingShader* shader : instancingAnimatedModelShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CUiShader* shader : instancingNumberUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CUiShader* shader : instancingImageUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }

	if (m_pMinimapCamera)
	{
		m_pMinimapCamera->ReleaseShaderVariables();
		delete m_pMinimapCamera;
	}

	if (blurShader)
		blurShader->Release();
	if (motionBlurShader)
		motionBlurShader->Release();
	if (m_pMinimapShader)
	{
		m_pMinimapShader->ReleaseShaderVariables();
		m_pMinimapShader->ReleaseObjects();
		m_pMinimapShader->Release();
	}
	if (m_pIconShader)
	{
		m_pIconShader->ReleaseShaderVariables();
		m_pIconShader->ReleaseObjects();
		m_pIconShader->Release();
	}
	if (m_pEffectShader)
	{
		m_pEffectShader->ReleaseShaderVariables();
		m_pEffectShader->ReleaseObjects();
		m_pEffectShader->Release();
	}
	if (m_pGuageShader)
	{
		m_pGuageShader->ReleaseShaderVariables();
		m_pGuageShader->ReleaseObjects();
		m_pGuageShader->Release();
	}
	UpdatedShaders.clear();
	instancingNumberUiShaders.clear();
	instancingImageUiShaders.clear();
	instancingBillBoardShaders.clear();
	instancingModelShaders.clear();
	instancingAnimatedModelShaders.clear();
}
void GameScene::BuildObjects(shared_ptr<CreateManager> pCreateManager)
{
	m_pCreateManager = pCreateManager;
	m_pNetWorkManager = pCreateManager->GetNetWorkMgr();
	m_pSoundManager = pCreateManager->GetSoundMgr();

	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	XMFLOAT3 xmf3Scale(TerrainScaleX, TerrainScaleY, TerrainScaleZ);
	
	m_pSkyBox = new SkyBoxObject(pCreateManager.get());
	
	m_pTerrain = new CHeightMapTerrain(pCreateManager.get(), _T("Resources\\Images\\First_Map.raw"), 257, 257, 7,
		7, xmf3Scale);

	CObInstancingShader* shader;
	CUiShader* uiShader;
	CSkinedObInstancingShader* animatedShader;

	UI_INFO view_info;    //게임중 or 대기중 뷰
	MODEL_INFO model_info;
	model_info.updatedContext = m_pTerrain;

	view_info.textureName = "Resources/Images/T_Gauge_Frame.dds";
	view_info.meshSize = XMFLOAT2(0.37f, 0.11f);
	view_info.positions.emplace_back(XMFLOAT3(-0.033f, -0.844f, 0.0f));
	view_info.maxUv = XMFLOAT2(1.0f, 1.0f);
	view_info.minUv = XMFLOAT2(0.0f, 0.0f);
	view_info.f_uvY.emplace_back(0.0f);
	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &view_info);
	instancingImageUiShaders.emplace_back(uiShader);
	view_info.positions.clear();
	view_info.f_uvY.clear();
	

	view_info.textureName = "Resources/Images/T_Blureffect.dds";
	view_info.meshSize = XMFLOAT2(1.0f, 1.0f);
	view_info.positions.emplace_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
	view_info.maxUv = XMFLOAT2(0.125f, 1.0f);
	view_info.minUv = XMFLOAT2(0.0f, 0.0f);
	view_info.f_uvY.emplace_back(0);
	m_pEffectShader = new ImageShader;
	m_pEffectShader->BuildObjects(pCreateManager.get(), &view_info);


	shader = new BillBoardShader;
	model_info.modelName = "Resources/Images/B_Tree.dds";
	model_info.dataFileName = "Resources/ObjectData/BillBoardData";
	model_info.size = 50;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingBillBoardShaders.emplace_back(shader);
	
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Tree.bin";
	model_info.dataFileName = "Resources/ObjectData/TreeData";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader); 

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Stone.bin";
	model_info.dataFileName = "Resources/ObjectData/StoneData";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Bush.bin";
	model_info.dataFileName = "Resources/ObjectData/WeedData";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new FenceShader;
	model_info.modelName = "Resources/Models/M_Block.bin";
	model_info.dataFileName = "Resources/ObjectData/RectData(Fence)";
	model_info.useBillBoard = false;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	m_pCheckPointShader = new BlockShader;
	model_info.modelName = "Resources/Models/M_Block.bin";
	model_info.dataFileName = "Resources/ObjectData/RectData(LineBox)";
	m_pCheckPointShader->BuildObjects(pCreateManager.get(), &model_info);
	//m_pCheckPointShader->AddRef();
	
	shader = new MeatShader;
	model_info.modelName = "Resources/Models/M_Meat.bin";
	model_info.dataFileName = "Resources/ObjectData/MeatData";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	
	m_pGuageShader = new GaugeShader;
	m_pGuageShader->BuildObjects(pCreateManager.get(), m_pTerrain);


	uiShader = new TimeCountShader;
	uiShader->BuildObjects(pCreateManager.get(), m_pTerrain);
	instancingNumberUiShaders.emplace_back(uiShader);

	uiShader = new TrackCountShader;
	uiShader->BuildObjects(pCreateManager.get(), m_pTerrain);
	instancingNumberUiShaders.emplace_back(uiShader);

	uiShader = new RankCountShader;
	uiShader->BuildObjects(pCreateManager.get(), m_pTerrain);
	instancingNumberUiShaders.emplace_back(uiShader);
	

	
	

	//animatedShader = new PlayerShader;
	//animatedShader->BuildObjects(pCreateManager.get(), "Resources/Models/Dino.bin", NULL);
	//instancingAnimatedModelShaders.emplace_back(animatedShader);
	//UpdatedShaders.emplace_back(animatedShader);
	m_pMinimapShader = new MinimapShader();
	m_pMinimapShader->BuildObjects(pCreateManager.get(), "Resources/Images/MiniMap.dds",NULL);

	string name = "Resources/Images/T_Faceicon.dds";
	m_pIconShader = new IconShader();
	m_pIconShader->BuildObjects(pCreateManager.get(), &name);

	blurShader = new BlurShader(pCreateManager.get());
	motionBlurShader = new MotionBlurShader(pCreateManager.get());

	XMFLOAT3 startPosition = m_pCheckPointShader->getList()[0]->GetPosition();
	particleSystems.emplace_back(new ParticleSystem(pCreateManager.get(), "Spawn", NULL, XMFLOAT3(startPosition.x, m_pTerrain->GetHeight(startPosition.x, startPosition.z), startPosition.z)));
	particleSystems.emplace_back(new ParticleSystem(pCreateManager.get(), "Spawn", NULL, XMFLOAT3(startPosition.x - 50, m_pTerrain->GetHeight(startPosition.x, startPosition.z), startPosition.z)));
	particleSystems.emplace_back(new ParticleSystem(pCreateManager.get(), "Spawn", NULL, XMFLOAT3(startPosition.x + 50, m_pTerrain->GetHeight(startPosition.x, startPosition.z), startPosition.z)));
	BuildLights();

	BuildSubCameras(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	CreateShaderVariables(pCreateManager.get());
	m_pSoundManager->Play("InGame_BGM", 0.2f);
}

void GameScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
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
void GameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_LEFT:
			if (m_pPlayer)  m_pPlayer->KeyUpLeft();
			break;
		case VK_RIGHT:
			if (m_pPlayer)	m_pPlayer->KeyUpRight();
			break;
		case VK_UP:
			if (m_pPlayer)	m_pPlayer->KeyUpUp();
			break;
		case VK_DOWN:
			if (m_pPlayer)	m_pPlayer->KeyUpDown();
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_SHIFT:
			if (m_pPlayer) 
				m_pPlayer->setShift(false);
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_CONTROL:
			break;
		case VK_F2:
		case VK_F3:
			if (m_pPlayer) m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1),
				deltaTime);
		case VK_LEFT:
			if (m_pPlayer) m_pPlayer->KeyDownLeft();
			break;
		case VK_RIGHT:
			if (m_pPlayer) m_pPlayer->KeyDownRight();
			break;
		case VK_UP:
			if (m_pPlayer)	m_pPlayer->KeyDownUp();
			break;
		case VK_DOWN:
			if (m_pPlayer)	m_pPlayer->KeyDownDown();
			break;
		case VK_SHIFT:
			if (m_pPlayer) 
				m_pPlayer->setShift(true);
			break;
		default:
			break;
		}
	default:
		break;
	}
}
void GameScene::ProcessInput(HWND hwnd, float deltaTime)
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
	/*마우스를 캡쳐했으면 마우스가 얼마만큼 이동하였는 가를 계산한다. 마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의
	메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다. 그러므로 마우스가 캡쳐된
	것은 마우스 버튼이 눌려진 상태를 의미한다. 마우스 버튼이 눌려진 상태에서 마우스를 좌우 또는 상하로 움직이면 플
	레이어를 x-축 또는 y-축으로 회전한다.*/
	if (::GetCapture() == hwnd)
	{
		/*
		//마우스 커서를 화면에서 없앤다(보이지 않게 한다).
		::SetCursor(NULL);
		//현재 마우스 커서의 위치를 가져온다. 
		::GetCursorPos(&ptCursorPos);
		//마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다. 
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정한다. 
		//m_ptOldCursorPos = ptCursorPos;
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		*/
	}
	//마우스 또는 키 입력이 있으면 플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			/*cxDelta는 y-축의 회전을 나타내고 cyDelta는 x-축의 회전을 나타낸다. 오른쪽 마우스 버튼이 눌려진 경우
			cxDelta는 z-축의 회전을 나타낸다.*/
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		/*플레이어를 dwDirection 방향으로 이동한다(실제로는 속도 벡터를 변경한다).
		이동 거리는 시간에 비례하도록 한다. 플레이어의 이동 속력은 (50/초)로 가정한다.*/
		if (dwDirection) m_pPlayer->Move(dwDirection, 20.0f,deltaTime,
			true);

	}
	else
		m_pPlayer->m_fForce = 0;
	//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다. 
	//m_pPlayer->FixedUpdate(deltaTime);
}

void GameScene::Render()
{
	BaseScene::Render();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_CUBE_MAP]);
	if (m_pSkyBox) m_pSkyBox->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SKIN_MESH]);
	m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_MODEL_INSTANCING]);
	for (CObInstancingShader* shader : instancingModelShaders)
	{
		if (shader)
			shader->Render(m_pd3dCommandList, m_pCamera);
	}
	m_pCheckPointShader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_BILLBOARD]);
	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);
	for (CObInstancingShader* shader : instancingModelShaders)
	{
		if (shader)
			shader->BillBoardRender(m_pd3dCommandList, m_pCamera);
	}

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_TERRAIN]);
	if (m_pTerrain) m_pTerrain->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SKIN_MESH_INSTANCING]);
	for (CSkinedObInstancingShader* shader : instancingAnimatedModelShaders)
		if (shader) {
			shader->Render(m_pd3dCommandList, m_pCamera);
		}

	
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PARTICLE]);
	//particleSystem->Render(m_pd3dCommandList.Get(), m_pCamera);
	m_pPlayer->m_pParticleSystem->Render(m_pd3dCommandList, m_pCamera);
	for (ParticleSystem* system : particleSystems)
	{
		system->Render(m_pd3dCommandList, m_pCamera);
	}
#ifdef _WITH_BOUND_BOX
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_WIRE]);
	m_pPlayer->BbxRender(m_pd3dCommandList, m_pCamera);
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_WIRE_INSTANCING]);
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) shader->BbxRender(m_pd3dCommandList, m_pCamera);

#endif
}

void GameScene::RenderShadow()
{
	BaseScene::RenderShadow();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_SKIN_MESH]);
	m_pPlayer->Render(m_pd3dCommandList, m_pShadowCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_MODEL_INSTANCING]);
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pShadowCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_BILLBOARD]);
	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pShadowCamera);
	for (CObInstancingShader* shader : instancingModelShaders)
	{
		if (shader)
			shader->BillBoardRender(m_pd3dCommandList, m_pShadowCamera);
	}
}
void GameScene::RenderVelocity()
{
	BaseScene::Render();

	//m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_VELOCITY_SKIN_MESH]);
	//m_pPlayer->Render(m_pd3dCommandList, m_pCamera);
	//
	//m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_VELOCITY_MODEL_INSTANCING]);
	//for (CObInstancingShader* shader : instancingModelShaders)
	//	if (shader) shader->Render(m_pd3dCommandList, m_pCamera);
	//
	//m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_VELOCITY_BILLBOARD]);
	//for (CObInstancingShader* shader : instancingBillBoardShaders)
	//	if (shader) shader->Render(m_pd3dCommandList, m_pCamera);
	//for (CObInstancingShader* shader : instancingModelShaders)
	//{
	//	if (shader)
	//		shader->BillBoardRender(m_pd3dCommandList, m_pCamera);
	//}
}

void GameScene::RenderPostProcess(ComPtr<ID3D12Resource> curBuffer, ComPtr<ID3D12Resource> velocityMap)
{
	static float deltaUvX = 0.0f;
	XMFLOAT3 vel = m_pPlayer->GetVelocity();
	float length = sqrtf(vel.x * vel.x + vel.z * vel.z);
	if (length > 30)
	{
		int idx = length - 30;
		blurShader->Dispatch(m_pd3dCommandList, m_ppd3dPipelineStates[PSO_HORZ_BLUR], m_ppd3dPipelineStates[PSO_VERT_BLUR], curBuffer.Get(), idx/10);
		//m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_MOTION_BLUR]);
		//motionBlurShader->Dispatch(m_pd3dCommandList, curBuffer.Get(), velocityMap.Get(), 10);


		
		m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_EFFECT]);
		m_pEffectShader->Render(m_pd3dCommandList, m_pCamera);
		m_pEffectShader->getUvXs()[0] = deltaUvX;
		deltaUvX += 0.125f;
		if (deltaUvX >= 1.0)
			deltaUvX = 0.0f;
	}

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	for (CUiShader* shader : instancingImageUiShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI_GAUGE]);
	if (m_pGuageShader)
		m_pGuageShader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI_NUMBER]);
	for (CUiShader* shader : instancingNumberUiShaders)
	{
		if (shader)
			shader->Render(m_pd3dCommandList, m_pCamera);
	}


	m_pMinimapCamera->SetViewportsAndScissorRects(m_pd3dCommandList);
	m_pMinimapCamera->UpdateShaderVariables(m_pd3dCommandList);
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_MINIMAP]);
	if (m_pMinimapShader)
		m_pMinimapShader->Render(m_pd3dCommandList, m_pMinimapCamera);
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	if(m_pIconShader)
		m_pIconShader->Render(m_pd3dCommandList, m_pMinimapCamera);

}
void GameScene::AnimateObjects(float fTimeElapsed)  
{
	//말 그대로 애니메이션 update
	if (m_pPlayer)
		m_pPlayer->Animate(fTimeElapsed);

	for (CSkinedObInstancingShader* shader : instancingAnimatedModelShaders)
		if (shader) { shader->AnimateObjects(fTimeElapsed); }
}


void GameScene::FixedUpdate(CreateManager* pCreateManager, float fTimeElapsed)
{
	//물리
	if (isStart)
	{
		m_pPlayer->FixedUpdate(fTimeElapsed);

		for (CObjectsShader* shader : UpdatedShaders)
		{
			shader->FixedUpdate(fTimeElapsed);  //물리 적용할 것
		}
	}
}


SceneType GameScene::Update(CreateManager* pCreateManager, float fTimeElapsed)
{
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PARTICLE_CALC]);

	m_pPlayer->m_pParticleSystem->AnimateObjects(fTimeElapsed);

	for (list<ParticleSystem*>::iterator i = particleSystems.begin(); i != particleSystems.end();)
	{
		if ((*i)->AnimateObjects(fTimeElapsed))
		{
			(*i)->Release();
			i = particleSystems.erase(i);
			if (i == particleSystems.end())
				break;
		}
		else
			i++;
	}

	if (isStart)
	{
		if (m_pPlayer->GetCheckPoint() == CHECKPOINT_GOAL)
		{
			sceneType = End_Scene;  //멀티 플레이시 이 구간에서 서버로부터 골인한 플레이어를 확인후 씬 전환
		}

		//충돌을 위한 update
		if (sceneType != SceneType::Game_Scene)
		{
			return sceneType;
		}
		if (m_pLights)
		{
			m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
			m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
		}
		MessageStruct message;
		for (CObjectsShader* shader : UpdatedShaders)
		{
			for (auto p = begin(shader->getList()); p < end(shader->getList());)
			{
				//플레이어 충돌처리할 곳
				if (m_pPlayer->IsCollide(*p))
				{
					if (m_pPlayer->Update(fTimeElapsed, *p))  //true반환 시 충돌된 오브젝트는 리스트에서 삭제
					{
						if ((*p)->GetModelType() == Fence || (*p)->GetModelType() == Player)
						{
							message.shaderName = "HeatEffect";
							message.departMat = m_pPlayer->m_xmf4x4World;
							message.msgName = "Add_Particle";
							EventHandler::GetInstance()->CallBack(message);
							m_pSoundManager->Play("Heat", 0.2f);
							p++;
						}
						else if ((*p)->GetModelType() == Item_Meat)
						{
							message.shaderName = "MeatParticle";
							message.departMat = (*p)->m_xmf4x4World;
							message.msgName = "Add_Particle";
							EventHandler::GetInstance()->CallBack(message);
							m_pSoundManager->Play("MeatEat", 0.5f);
							p++;
						}
						else
						{
							(*p)->Release();
							p = shader->getList().erase(p);
						}
					}
					else
						p++;
					//타겟 오브젝트 타입 구해오고 경우에 맞게 처리하도록 작성할 것
				}
				else
					p++;
			}
			shader->Update(fTimeElapsed);  //물리 적용할 것
		}

		//체크포인트 충돌처리
		if (m_pCheckPointShader)
		{
			UINT currentCheckPoint = m_pPlayer->GetCheckPoint();
			CGameObject* checkPoint = m_pCheckPointShader->getList()[currentCheckPoint % 181];
			if (m_pPlayer->IsCollide(checkPoint))
				m_pPlayer->UpCheckPoint();
		}

		if (m_pGuageShader)
			m_pGuageShader->Update(fTimeElapsed, m_pPlayer);

		for (CUiShader* shader : instancingNumberUiShaders)
			shader->Update(fTimeElapsed, m_pPlayer);

		XMFLOAT3 playerPosition = m_pPlayer->GetPosition();
		if (m_pMinimapCamera)
		{
			m_pMinimapCamera->SetPosition(XMFLOAT3(playerPosition.x, 300, playerPosition.z));
			m_pMinimapCamera->RegenerateViewMatrix();
		}
	}
	else
	{
		isStart = true;
		//서버에서 스타트 신호를 받음. 모든 유저가 접속이 되었다면. start신호를 받고 isStart값을 true로 전환하고 
		//start대문이미지 출력할 것.
	}
	return Game_Scene;
}

void GameScene::BuildLights()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.6f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 50.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 1.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->fogstart = 25;
	m_pLights->fogrange = 30;

}
void GameScene::BuildSubCameras(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	m_pMinimapCamera = new CMinimapCamera;
	m_pMinimapCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pMinimapCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pMinimapCamera->GenerateOrthoProjectionMatrix(1000, 1000, 10, 300.0f);
	m_pMinimapCamera->SetViewport(FRAME_BUFFER_WIDTH - 250, FRAME_BUFFER_HEIGHT - 180, 250, 180, 0.0f, 1.0f);
	m_pMinimapCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	m_pMinimapCamera->GenerateViewMatrix(m_pMinimapCamera->GetPosition(), XMFLOAT3(128 * TerrainScaleX, 0, 128 * TerrainScaleZ), XMFLOAT3(0, 0, 1));
	m_pMinimapCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pShadowCamera = new CMinimapCamera;
	m_pShadowCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pShadowCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pShadowCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pShadowCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
}

void GameScene::UpdateShadow()
{
	XMFLOAT3 centerPosition(m_pPlayer->GetPosition());  //지형의 한 가운데
	float rad = 1000;   // 지형을 담는 구의 반지름(ex 지구의 반지름)

	XMVECTOR lightDir = XMLoadFloat3(&m_pLights->m_pLights[0].m_xmf3Direction);
	lightDir = XMVector3Normalize(lightDir);

	XMVECTOR shadowCameraPosition = XMLoadFloat3(&centerPosition) - 2.0f*rad*lightDir;
	XMVECTOR targetPosition = XMLoadFloat3(&centerPosition);
	XMVECTOR shadowUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMFLOAT3 xmf3CameraPosition;
	XMFLOAT3 xmf3CameraTarget;
	XMStoreFloat3(&xmf3CameraPosition, shadowCameraPosition);
	XMStoreFloat3(&xmf3CameraTarget, targetPosition);

	m_pShadowCamera->GenerateViewMatrix(xmf3CameraPosition, xmf3CameraTarget, XMFLOAT3(0, 1, 0));
	m_pShadowCamera->GenerateFrustum();

#ifdef _WITH_LEFT_HAND_COORDINATES
	XMMATRIX shadowView = XMMatrixLookAtLH(shadowCameraPosition, targetPosition, shadowUp);
#else
	XMMATRIX shadowView = XMMatrixLookAtLH(shadowCameraPosition, targetPosition, shadowUp);
#endif

	XMStoreFloat3(&m_pcbMappedShadow->m_xmf3ShadowCameraPosition, shadowCameraPosition);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPosition, shadowView));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - rad;
	float b = sphereCenterLS.y - rad;
	float n = sphereCenterLS.z - rad;
	float r = sphereCenterLS.x + rad;
	float t = sphereCenterLS.y + rad;
	float f = sphereCenterLS.z + rad;

#ifdef _WITH_LEFT_HAND_COORDINATES
	XMMATRIX ShadowProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
#else
	XMMATRIX ShadowProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
#endif
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = shadowView * ShadowProj*T;
	XMStoreFloat4x4(&m_pcbMappedShadow->m_xmf4x4ShadowView, XMMatrixTranspose(shadowView));
	XMStoreFloat4x4(&m_pcbMappedShadow->m_xmf4x4ShadowProjection, XMMatrixTranspose(ShadowProj));
	XMStoreFloat4x4(&m_pcbMappedShadow->m_xmf4x4InvShadowViewProjection, XMMatrixTranspose(S));
	XMFLOAT4X4 martrix;
	XMStoreFloat4x4(&martrix, ShadowProj);
	m_pShadowCamera->SetProjectionMatrix(martrix);
}
void GameScene::CreateShaderVariables(CreateManager* pCreateManager)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pCreateManager->GetDevice().Get(), m_pd3dCommandList, NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	ncbElementBytes = ((sizeof(CB_GAME_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbShadow = ::CreateBufferResource(pCreateManager->GetDevice().Get(), m_pd3dCommandList, NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbShadow->Map(0, NULL, (void **)&m_pcbMappedShadow);

}


void GameScene::UpdateShaderVariables()
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbShadowGpuVirtualAddress =
		m_pd3dcbShadow->GetGPUVirtualAddress();

	m_pd3dCommandList->SetGraphicsRootConstantBufferView(1, d3dcbShadowGpuVirtualAddress);
	UpdateShadow();
}

void GameScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}

	if (m_pd3dcbShadow)
	{
		m_pd3dcbShadow->Unmap(0, NULL);
		m_pd3dcbShadow->Release();
	}
}


void GameScene::setPlayer(CPlayer* player)
{
	BaseScene::setPlayer(player);
	player->SetUpdatedContext((CHeightMapTerrain*)m_pTerrain);
}

void GameScene::setCamera(CCamera* camera)
{
	BaseScene::setCamera(camera);
	m_pPlayer->SetCameraUpdatedContext((CHeightMapTerrain*)m_pTerrain);
}

void GameScene::ResetShadowBuffer(CreateManager* pCreateManager)
{
	m_pTerrain->resetShadowTexture(pCreateManager);
}

void GameScene::AddParticle(const MessageStruct& msg)
{
	XMFLOAT3 pos = XMFLOAT3(msg.departMat._41, msg.departMat._42, msg.departMat._43);
	particleSystems.emplace_back(new ParticleSystem(m_pCreateManager.get(), msg.shaderName, NULL, pos));
}