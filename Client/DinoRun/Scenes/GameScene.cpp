#include "GameScene.h"
#include "../Common/FrameWork/CreateManager.h"

#include "../Objects/PlayerObject.h"
#include "../Objects/SkyBoxObject.h"
#include "../Objects/TerrainObject.h"


#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/ModelShader/ModelShader.h"
#include "../CShaders/SkinedShader/SkinedShader.h"
#include "../CShaders/BlurShader/BlurShader.h"
#include "../CShaders/MinimapShader/MinimapShader.h"

#include "../Common/ParticleSystem/ParticleSystem.h"

#include "../CShaders/UiShader/UiShader.h"


#include "../Common/Camera/Camera.h"

GameScene::GameScene() :BaseScene()
{
	sceneType = SceneType::Game_Scene;
}
GameScene::~GameScene()
{

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

	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) shader->ReleaseUploadBuffers();
	for (CObInstancingShader* shader : instacingModelShaders)
		if (shader) shader->ReleaseUploadBuffers();
	for (CSkinedObInstancingShader* shader : instacingAnimatedModelShaders)
		if (shader) shader->ReleaseUploadBuffers();
	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }

	if (m_pMinimapShader)
		m_pMinimapShader->ReleaseUploadBuffers();
	if (m_pIconShader)
		m_pIconShader->ReleaseUploadBuffers();
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

	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CObInstancingShader* shader : instacingModelShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CSkinedObInstancingShader* shader : instacingAnimatedModelShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }

	if (m_pMinimapCamera)
	{
		m_pMinimapCamera->ReleaseShaderVariables();
		delete m_pMinimapCamera;
	}

	if (blurShader)
		blurShader->Release();

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

	UpdatedShaders.clear();
	instacingUiShaders.clear();
	instacingBillBoardShaders.clear();
	instacingModelShaders.clear();
	instacingAnimatedModelShaders.clear();
}
void GameScene::BuildObjects(shared_ptr<CreateManager> pCreateManager)
{

	ComPtr<ID3D12Device> m_pd3dDevice = pCreateManager->GetDevice();
	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	XMFLOAT3 xmf3Scale(TerrainScaleX, TerrainScaleY, TerrainScaleZ);
	
	m_pSkyBox = new SkyBoxObject(pCreateManager);
	
	m_pTerrain = new CHeightMapTerrain(pCreateManager, _T("Resources\\Images\\First_Map.raw"), 257, 257, 17,
		17, xmf3Scale);

	CObInstancingShader* shader;
	CUiShader* uiShader;
	//CSkinedObInstancingShader* animatedShader;
	
	shader = new BillBoardShader;
	shader->BuildObjects(pCreateManager, "Resources/Images/treearray.dds", "Resources/ObjectData/BillBoardData");
	instacingBillBoardShaders.emplace_back(shader);
	
	shader = new TreeShader;
	shader->BuildObjects(pCreateManager, "Resources/Models/Tree1.bin", "Resources/ObjectData/TreeData");
	instacingModelShaders.emplace_back(shader); 
	
	shader = new FenceShader;
	shader->BuildObjects(pCreateManager, "Resources/Models/Block.bin", "Resources/ObjectData/RectData(Fence)");
	instacingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	m_pCheckPointShader = new BlockShader;
	m_pCheckPointShader->BuildObjects(pCreateManager, "Resources/Models/Block.bin", "Resources/ObjectData/RectData(LineBox)");
	m_pCheckPointShader->AddRef();
	
	shader = new ItemShader;
	shader->BuildObjects(pCreateManager, "Resources/Models/Item_Meat.bin", "Resources/ObjectData/MeatData");
	instacingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	
	uiShader = new GaugeShader;
	uiShader->BuildObjects(pCreateManager, m_pTerrain);
	instacingUiShaders.emplace_back(uiShader);

	uiShader = new TimeCountShader;
	uiShader->BuildObjects(pCreateManager, m_pTerrain);
	instacingUiShaders.emplace_back(uiShader);

	uiShader = new TrackCountShader;
	uiShader->BuildObjects(pCreateManager, m_pTerrain);
	instacingUiShaders.emplace_back(uiShader);
	
	//animatedShader = new PlayerShader;
	//animatedShader->BuildObjects(pCreateManager, "Resources/Models/Dino.bin", "Resources/ObjectData/TreeData");
	//instacingAnimatedModelShaders.emplace_back(animatedShader);
	//animatedShader->AddRef();
	//UpdatedShaders.emplace_back(animatedShader);
	m_pMinimapShader = new MinimapShader();
	m_pMinimapShader->BuildObjects(pCreateManager, "Resources/Images/MiniMap.dds",NULL);

	string name = "Resources/Images/Face_Icon.dds";
	m_pIconShader = new IconShader();
	m_pIconShader->BuildObjects(pCreateManager, &name);

	blurShader = new BlurShader(pCreateManager);

	particleSystems.emplace_back(new ParticleSystem(pCreateManager, ONES, RAND, 1.8f, 0.5, NULL, XMFLOAT3(800.0f, 80, 940),
		15, "Resources/Images/smoke.dds", 2,30));

	particleSystems.emplace_back(new ParticleSystem(pCreateManager, ONES, RAND, 1.8f, 0.5, NULL, XMFLOAT3(750.0f, 80, 900),
		15, "Resources/Images/smoke.dds", 5,50));
	BuildLights();

	BuildMinimapCamera(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	CreateShaderVariables(pCreateManager);
}

void GameScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_RBUTTONUP:
		//���콺 ĸ�ĸ� �����Ѵ�. 

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
	/*Ű������ ���� ������ ��ȯ�Ѵ�. ȭ��ǥ Ű(���桯, ���硯, ���衯, ���顯)�� ������ �÷��̾ ������/����(���� x-��), ��/
	��(���� z-��)�� �̵��Ѵ�. ��Page Up���� ��Page Down�� Ű�� ������ �÷��̾ ��/�Ʒ�(���� y-��)�� �̵��Ѵ�.*/
	if (::GetKeyboardState(pKeyBuffer))
	{
		// ��
		if (pKeyBuffer[VK_UP] & 0xF0)
			dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}
	float cxDelta = 0.0f, cyDelta = 0.0f;
	/*���콺�� ĸ�������� ���콺�� �󸶸�ŭ �̵��Ͽ��� ���� ����Ѵ�. ���콺 ���� �Ǵ� ������ ��ư�� ������ ����
	�޽���(WM_LBUTTONDOWN, WM_RBUTTONDOWN)�� ó���� �� ���콺�� ĸ���Ͽ���. �׷��Ƿ� ���콺�� ĸ�ĵ�
	���� ���콺 ��ư�� ������ ���¸� �ǹ��Ѵ�. ���콺 ��ư�� ������ ���¿��� ���콺�� �¿� �Ǵ� ���Ϸ� �����̸� ��
	���̾ x-�� �Ǵ� y-������ ȸ���Ѵ�.*/
	if (::GetCapture() == hwnd)
	{
		/*
		//���콺 Ŀ���� ȭ�鿡�� ���ش�(������ �ʰ� �Ѵ�).
		::SetCursor(NULL);
		//���� ���콺 Ŀ���� ��ġ�� �����´�. 
		::GetCursorPos(&ptCursorPos);
		//���콺 ��ư�� ���� ���¿��� ���콺�� ������ ���� ���Ѵ�. 
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//���콺 Ŀ���� ��ġ�� ���콺�� �������� ��ġ�� �����Ѵ�. 
		//m_ptOldCursorPos = ptCursorPos;
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		*/
	}
	//���콺 �Ǵ� Ű �Է��� ������ �÷��̾ �̵��ϰų�(dwDirection) ȸ���Ѵ�(cxDelta �Ǵ� cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			/*cxDelta�� y-���� ȸ���� ��Ÿ���� cyDelta�� x-���� ȸ���� ��Ÿ����. ������ ���콺 ��ư�� ������ ���
			cxDelta�� z-���� ȸ���� ��Ÿ����.*/
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		/*�÷��̾ dwDirection �������� �̵��Ѵ�(�����δ� �ӵ� ���͸� �����Ѵ�).
		�̵� �Ÿ��� �ð��� ����ϵ��� �Ѵ�. �÷��̾��� �̵� �ӷ��� (50/��)�� �����Ѵ�.*/
		if (dwDirection) m_pPlayer->Move(dwDirection, 50.0f,
			true);

	}
	else
		m_pPlayer->m_fForce = 0;
	//�÷��̾ ������ �̵��ϰ� ī�޶� �����Ѵ�. �߷°� �������� ������ �ӵ� ���Ϳ� �����Ѵ�. 
	//m_pPlayer->FixedUpdate(deltaTime);
}

void GameScene::Render(float fTimeElapsed)
{
	BaseScene::Render(fTimeElapsed);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_CUBE_MAP]);
	if (m_pSkyBox) m_pSkyBox->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SKIN_MESH]);
	m_pPlayer->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_MODEL_INSTANCING]);
	for (CObInstancingShader* shader : instacingModelShaders)
	{
		if (shader)
			shader->Render(m_pd3dCommandList.Get(), m_pCamera);
	}
	m_pCheckPointShader->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_BILLBOARD]);
	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_TERRAIN]);
	if (m_pTerrain) m_pTerrain->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SKIN_MESH_INSTANCING]);
	for (CSkinedObInstancingShader* shader : instacingAnimatedModelShaders)
		if (shader) {
			shader->Render(m_pd3dCommandList.Get(), m_pCamera);
		}

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PARTICLE_CALC]);
	//particleSystem->AnimateObjects(fTimeElapsed);
	m_pPlayer->m_pParticleSystem->AnimateObjects(fTimeElapsed);
	
	for(list<ParticleSystem*>::iterator i = particleSystems.begin(); i != particleSystems.end();)
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
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PARTICLE]);
	//particleSystem->Render(m_pd3dCommandList.Get(), m_pCamera);
	m_pPlayer->m_pParticleSystem->Render(m_pd3dCommandList.Get(), m_pCamera);
	for (ParticleSystem* system : particleSystems)
	{
		system->Render(m_pd3dCommandList.Get(), m_pCamera);
	}
#ifdef _WITH_BOUND_BOX
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_WIRE]);
	m_pPlayer->BbxRender(m_pd3dCommandList.Get(), m_pCamera);
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_WIRE_INSTANCING]);
	for (CObInstancingShader* shader : instacingModelShaders)
		if (shader) shader->BbxRender(m_pd3dCommandList.Get(), m_pCamera);

#endif
}

void GameScene::RenderShadow()
{
	BaseScene::RenderShadow();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_SKIN_MESH]);
	m_pPlayer->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_MODEL_INSTANCING]);
	for (CObInstancingShader* shader : instacingModelShaders)
		if (shader) shader->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_BILLBOARD]);
	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList.Get(), m_pCamera);
}
void GameScene::RenderPostProcess(ComPtr<ID3D12Resource> curBuffer)
{
	//blurShader->Dispatch(m_pd3dCommandList.Get(), m_ppd3dPipelineStates[17], m_ppd3dPipelineStates[18], curBuffer.Get(), 1);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI_GAUGE]);
	if (instacingUiShaders[0])
		instacingUiShaders[0]->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI_NUMBER]);
	if (instacingUiShaders[1])
		instacingUiShaders[1]->Render(m_pd3dCommandList.Get(), m_pCamera);
	if (instacingUiShaders[2])
		instacingUiShaders[2]->Render(m_pd3dCommandList.Get(), m_pCamera);

	m_pMinimapCamera->SetViewportsAndScissorRects(m_pd3dCommandList.Get());
	m_pMinimapCamera->UpdateShaderVariables(m_pd3dCommandList.Get());
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_MINIMAP]);
	if (m_pMinimapShader)
		m_pMinimapShader->Render(m_pd3dCommandList.Get(), m_pMinimapCamera);
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	if(m_pIconShader)
		m_pIconShader->Render(m_pd3dCommandList.Get(), m_pMinimapCamera);
}
void GameScene::AnimateObjects(float fTimeElapsed)  
{
	//�� �״�� �ִϸ��̼� update
	if (m_pPlayer)
		m_pPlayer->Animate(fTimeElapsed);

	for (CSkinedObInstancingShader* shader : instacingAnimatedModelShaders)
		if (shader) { shader->AnimateObjects(fTimeElapsed); }
}

SceneType GameScene::Update(float fTimeElapsed)
{
	if (m_pPlayer->GetCheckPoint() == CHECKPOINT_GOAL)
	{
		return End_Scene;  //��Ƽ �÷��̽� �� �������� �����κ��� ������ �÷��̾ Ȯ���� �� ��ȯ
	}

	//�浹�� ���� update
	if (sceneType != SceneType::Game_Scene)
	{
		return sceneType;
	}
	if (m_pLights)
	{
		m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}

	for(CUiShader* shader : instacingUiShaders)
			shader->Update(fTimeElapsed, m_pPlayer);
	
	for (CObjectsShader* shader : UpdatedShaders)
	{
		for (auto p = begin(shader->getList());p< end(shader->getList());)
		{
			//�÷��̾� �浹ó���� ��
			if (m_pPlayer->IsCollide(*p))
			{
				if (m_pPlayer->Update(fTimeElapsed, *p))  //true��ȯ �� �浹�� ������Ʈ�� ����Ʈ���� ����
				{
					(*p)->Release();
					p = shader->getList().erase(p);
				}
				else
					p++;
				//Ÿ�� ������Ʈ Ÿ�� ���ؿ��� ��쿡 �°� ó���ϵ��� �ۼ��� ��
			}
			else
				p++;
		}
		shader->Update(fTimeElapsed);  //���� ������ ��
	}

	//üũ����Ʈ �浹ó��
	if (m_pCheckPointShader)
	{
		UINT currentCheckPoint = m_pPlayer->GetCheckPoint();
		CGameObject* checkPoint = m_pCheckPointShader->getList()[currentCheckPoint % 181];
		if (m_pPlayer->IsCollide(checkPoint))
			m_pPlayer->UpCheckPoint();
	}

	XMFLOAT3 playerPosition = m_pPlayer->GetPosition();
	if (m_pMinimapCamera)
	{
		m_pMinimapCamera->SetPosition(XMFLOAT3(playerPosition.x, 300, playerPosition.z));
		m_pMinimapCamera->RegenerateViewMatrix();
	}
	return Game_Scene;
}

void GameScene::FixedUpdate(float fTimeElapsed)
{
	//����
	m_pPlayer->FixedUpdate(fTimeElapsed);

	for (CObjectsShader* shader : UpdatedShaders)
	{
		shader->FixedUpdate(fTimeElapsed);  //���� ������ ��
	}
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
void GameScene::BuildMinimapCamera(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	m_pMinimapCamera = new CMinimapCamera;
	m_pMinimapCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pMinimapCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pMinimapCamera->GenerateOrthoProjectionMatrix(1000, 1000, 10, 300.0f);
	m_pMinimapCamera->SetViewport(FRAME_BUFFER_WIDTH - 250, FRAME_BUFFER_HEIGHT-180, 250, 180, 0.0f, 1.0f);
	m_pMinimapCamera->SetScissorRect(0,0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	m_pMinimapCamera->GenerateViewMatrix(m_pMinimapCamera->GetPosition(), XMFLOAT3(128 * TerrainScaleX, 0, 128 * TerrainScaleZ), XMFLOAT3(0, 0, 1));
	m_pMinimapCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void GameScene::UpdateShadow()
{	
	XMFLOAT3 centerPosition(m_pPlayer->GetPosition());  //������ �� ���
	float rad = 800;   // ������ ��� ���� ������(ex ������ ������)

	XMVECTOR lightDir = XMLoadFloat3(&m_pLights->m_pLights[0].m_xmf3Direction);
	lightDir= XMVector3Normalize(lightDir);

	XMVECTOR shadowCameraPosition = XMLoadFloat3(&centerPosition)-2.0f*rad*lightDir;
	XMVECTOR targetPosition = XMLoadFloat3(&centerPosition);
	XMVECTOR shadowUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
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
}
void GameScene::CreateShaderVariables(shared_ptr<CreateManager> pCreateManager)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	m_pd3dcbLights = ::CreateBufferResource(pCreateManager->GetDevice().Get(), m_pd3dCommandList.Get(), NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	ncbElementBytes = ((sizeof(CB_GAME_INFO) + 255) & ~255); //256�� ���
	m_pd3dcbShadow = ::CreateBufferResource(pCreateManager->GetDevice().Get(), m_pd3dCommandList.Get(), NULL,
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
	player->SetPlayerUpdatedContext((CHeightMapTerrain*)m_pTerrain);
}

void GameScene::setCamera(CCamera* camera)
{
	BaseScene::setCamera(camera);
	m_pPlayer->SetCameraUpdatedContext((CHeightMapTerrain*)m_pTerrain);
}

void GameScene::ResetShadowBuffer(shared_ptr<CreateManager> pCreateManager)
{
	m_pTerrain->resetShadowTexture(pCreateManager);
}