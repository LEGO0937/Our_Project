#include "GameScene.h"
#include "../Objects/PlayerObject.h"
#include "../Objects/SkyBoxObject.h"
#include "../Objects/TerrainObject.h"
#include "../CShaders/FenceShader/FenceShader.h"
#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/TreeShader/TreeShader.h"
#include "../Common/ImGui/imgui/imgui.h"
#include "../Common/Camera/Camera.h"

extern int numMesh;
extern float nScale;
GameScene::GameScene() :BaseScene()
{
}
GameScene::~GameScene()
{

}
void GameScene::ReleaseUploadBuffers()
{
	BaseScene::ReleaseUploadBuffers();
	if (m_pTerrain)
		m_pTerrain->ReleaseUploadBuffers();
	for (CObInstancingShader* shader : instacingShaders)
		if (shader) shader->ReleaseUploadBuffers();
}
void GameScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();
	if (m_pTerrain)
		m_pTerrain->Release();
	for (CObInstancingShader* shader : instacingShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release();}
	instacingShaders.clear();
}
void GameScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{

	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;

	XMFLOAT3 xmf3Scale(25.0f, 0.7f, 25.0f);
	m_pSkyBox = new SkyBoxObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Resources\\Images\\First_Map.raw"), 257, 257, 257,
		257, xmf3Scale);
	CObInstancingShader* shader;
	shader = new BillBoardShader;
	shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	instacingShaders.emplace_back(shader);

	shader = new TreeShader;
	shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	instacingShaders.emplace_back(shader);

	shader = new RectShader;
	shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	instacingShaders.emplace_back(shader);

	shader = new MFenceShader;
	shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	instacingShaders.emplace_back(shader);

	shader = new LFenceShader;
	shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	instacingShaders.emplace_back(shader);

	shader = new MeatShader;
	shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pTerrain);
	instacingShaders.emplace_back(shader);

	BuildLights();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
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

		//bulletShader->addBullet(m_pd3dDevice, m_pd3dCommandList, m_pPlayer->m_xmf4x4World);
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
		case VK_F1:
		case VK_F2:
		case VK_F3:
			//if (m_pPlayer) m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1),
			//	deltaTime);
			break;
		case VK_LEFT:
			if (m_pPlayer) 
				m_pPlayer->KeyUpLeft();
			break;
		case VK_RIGHT:
			if (m_pPlayer) 
				m_pPlayer->KeyUpRight();
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;		
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
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
	POINT ptCursorPos;
	/*마우스를 캡쳐했으면 마우스가 얼마만큼 이동하였는 가를 계산한다. 마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의
	메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다. 그러므로 마우스가 캡쳐된
	것은 마우스 버튼이 눌려진 상태를 의미한다. 마우스 버튼이 눌려진 상태에서 마우스를 좌우 또는 상하로 움직이면 플
	레이어를 x-축 또는 y-축으로 회전한다.*/
	if (::GetCapture() == hwnd)
	{
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
		if (dwDirection) m_pPlayer->Move(dwDirection, 610.0f*deltaTime,
			true);

	}
	//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다. 
	m_pPlayer->Update(deltaTime);

}

void GameScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	BaseScene::Render(pd3dCommandList);

	{
		ImGui::Begin("list UI");                          // Create a window called "Hello, world!" and append into it.

		if (ImGui::Button("Add"))
		{

			instacingShaders[numMesh]->Add(m_pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->m_xmf4x4ToParent);
			
		}
		//instacingShaders[numMesh]->Add(m_pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->m_xmf4x4ToParent);

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
			instacingShaders[numMesh]->Delete();

		if (ImGui::Button("Save"))
			instacingShaders[numMesh]->Save();

		ImGui::SameLine();

		if (ImGui::Button("Load"))
			instacingShaders[numMesh]->Load(m_pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		//
		//ImGui::SliderFloat("float", &f, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		//
		//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//	counter++;
		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);
		//
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[2]);
	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, m_pCamera);
	//m_pPlayer->OnPrepareRender();
//	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[8]);
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[1]);
	m_pPlayer->Render(pd3dCommandList, m_pCamera);
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[4]);
	instacingShaders[1]->Render(pd3dCommandList, m_pCamera);
	instacingShaders[2]->Render(pd3dCommandList, m_pCamera);
	instacingShaders[3]->Render(pd3dCommandList, m_pCamera);
	instacingShaders[4]->Render(pd3dCommandList, m_pCamera);
	instacingShaders[5]->Render(pd3dCommandList, m_pCamera);
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[7]);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, m_pCamera);
	//if (isStart)
	//{
	//	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[2]);
	//	cubemap->Render(pd3dCommandList, m_pCamera);
	//
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[3]);
	//for (CObInstancingShader* shader : instacingShaders)
	//	if (shader) shader->Render(pd3dCommandList, m_pCamera);
	instacingShaders[0]->Render(pd3dCommandList, m_pCamera);


	//	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[4]);
	//	shaders[1]->Render(pd3dCommandList, m_pCamera);
	//	bulletShader->Render(pd3dCommandList, m_pCamera);
	//
	//
	//	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[7]);
	//	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, m_pCamera);
	//	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
	//	m_pPlayer->Render(pd3dCommandList, m_pCamera);
	//	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[6]);
	//	if (m_pSurf) m_pSurf->Render(pd3dCommandList, m_pCamera);
	//}
	//else
	//{
	//	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[5]);
	//	ui->Render(pd3dCommandList, m_pCamera);
	//}
	
}

void GameScene::AnimateObjects(float fTimeElapsed)
{
	BaseScene::AnimateObjects(fTimeElapsed);

	if (m_pLights)
	{
		//m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		//m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void GameScene::Update(float fTimeElapsed)
{
	m_pPlayer->OnPrepareRender();
	
	if (m_pPlayer->GetCamera()->GetMode() != THIRD_PERSON_CAMERA)
		return;
	if (instacingShaders[numMesh])
	{
		if (instacingShaders[numMesh]->getList().size() != 0)
		{
			CGameObject* ob = *(instacingShaders[numMesh]->getList().begin());
			ob->m_xmf4x4ToParent = m_pPlayer->m_xmf4x4ToParent;
		}
	}
	//if (m_pLights)
	//{
	//	m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
	//	m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	//}
}

void GameScene::BuildLights()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 1500.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(0.0f, 500.0f, -25.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.1f, 0.01f);
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 1000.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.9f, 0.0f, 0.7f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(150.0f, 0.0f, 70.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 4.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 150.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(-150.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
	m_pLights->fogstart = 25;
	m_pLights->fogrange = 30;

}

void GameScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}


void GameScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
}

void GameScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}
