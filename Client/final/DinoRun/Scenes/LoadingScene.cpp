#include "LoadingScene.h"

#include "../Common/FrameWork/GameManager.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"

LoadingScene::LoadingScene() :BaseScene()
{
	sceneType = SceneType::Default_Scene;
}
LoadingScene::~LoadingScene()
{

}
void LoadingScene::ReleaseUploadBuffers()
{
	BaseScene::ReleaseUploadBuffers();

	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
}
void LoadingScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();

	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }

}
void LoadingScene::BuildObjects()
{
	m_pd3dCommandList = GameManager::GetInstance()->GetCommandList().Get();

	CUiShader* uiShader;

	UI_INFO Ui_info;

	//---- 아이템 틀
	Ui_info.textureName = "Resources/Images/T_Loading.dds";
	Ui_info.meshSize = XMFLOAT2(1.0f, 1.0f);
	Ui_info.positions.emplace_back(XMFLOAT3(0.0f, 0.0f, 0.1f));
	Ui_info.maxUv = XMFLOAT2(0.125f, 1.0f);
	Ui_info.minUv = XMFLOAT2(0.0f, 0.0f);
	Ui_info.f_uvY.emplace_back(0);

	uiShader = new ImageShader;
	uiShader->BuildObjects(&Ui_info);
	instacingUiShaders.emplace_back(uiShader);

	CreateShaderVariables();
}

void LoadingScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
}
void LoadingScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			//네트워크 클래스에 있는 방번호가 아직 있으므로 그 번호의 RoomScene으로 넘어가도록 한다.
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_BACK:
			break;

		default:
			break;
		}
		break;
	default:
		break;
	}
}
void LoadingScene::ProcessInput(HWND hwnd, float deltaTime)
{
}

void LoadingScene::Render()
{
	BaseScene::Render();
	
	static float deltaUvX = 0.0f;
	if (deltaUvX >= 1.0f)
		deltaUvX = 0.0f;
	//말 그대로 애니메이션 update
	instacingUiShaders[0]->getUvXs()[0] = deltaUvX;
	deltaUvX += 0.125f;

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	if (instacingUiShaders[0])
		instacingUiShaders[0]->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_FONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList, m_pCamera, gameTexts);

#ifdef _WITH_BOUND_BOX

#endif
}


void LoadingScene::AnimateObjects(float fTimeElapsed)
{
	
}

SceneType LoadingScene::Update(float fTimeElapsed)
{
	//물리 및 충돌을 위한 update
	if (sceneType != SceneType::End_Scene)
	{
		return sceneType;
	}

	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);
	return SceneType::End_Scene;
}


void LoadingScene::CreateShaderVariables()
{
}


void LoadingScene::UpdateShaderVariables()
{
}

void LoadingScene::ReleaseShaderVariables()
{

}
