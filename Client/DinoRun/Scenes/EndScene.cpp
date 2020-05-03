#include "EndScene.h"

#include "../Common/FrameWork/CreateManager.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"

EndScene::EndScene() :BaseScene()
{
	sceneType = SceneType::End_Scene;
}
EndScene::~EndScene()
{

}
void EndScene::ReleaseUploadBuffers()
{
	BaseScene::ReleaseUploadBuffers();

	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
}
void EndScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();

	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }

}
void EndScene::BuildObjects(CreateManager* pCreateManager)
{
	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name = "Resources/Images/Win.dds";
	uiShader->BuildObjects(pCreateManager, &name);
	instacingUiShaders.emplace_back(uiShader);

	CreateShaderVariables(pCreateManager);
}

void EndScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
}
void EndScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			sceneType = SceneType::Room_Scene;
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
void EndScene::ProcessInput(HWND hwnd, float deltaTime)
{
}

void EndScene::Render(float fTimeElapsed)
{
	BaseScene::Render(fTimeElapsed);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_BILLBOARD]);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	if (instacingUiShaders[0])
		instacingUiShaders[0]->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList, m_pCamera, gameTexts);

#ifdef _WITH_BOUND_BOX

#endif
}


void EndScene::AnimateObjects(float fTimeElapsed)
{
	//말 그대로 애니메이션 update

}

SceneType EndScene::Update(CreateManager* pCreateManager, float fTimeElapsed)
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


void EndScene::CreateShaderVariables(CreateManager* pCreateManager)
{
}


void EndScene::UpdateShaderVariables()
{
}

void EndScene::ReleaseShaderVariables()
{

}
