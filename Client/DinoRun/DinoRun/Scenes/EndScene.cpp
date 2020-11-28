#include "EndScene.h"

#include "../Common/FrameWork/GameManager.h"
#include "../Common/FrameWork/SoundManager.h"
#include "../Common/FrameWork/NetworkManager.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"
#include "EventHandler/EventHandler.h"

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

	for (CUiShader* shader : instancingNumberUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
}
void EndScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();

	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for(CUiShader* shader : instancingNumberUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
}
void EndScene::BuildObjects()
{
	SoundManager::GetInstance()->AllStop();

	m_pd3dCommandList = GameManager::GetInstance()->GetCommandList().Get();

	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name;
	if(NetWorkManager::GetInstance()->GetPlayerName() == EventHandler::GetInstance()->m_sWinner)
		name = "Resources/Images/T_Win.dds";
	else
		name = "Resources/Images/T_Lose.dds";
	
	uiShader->BuildObjects(&name);
	instacingUiShaders.emplace_back(uiShader);

	UI_INFO view_info;
	view_info.textureName = "Resources/Images/T_EndText.dds";
	view_info.meshSize = XMFLOAT2(0.1f, 0.085f);
	view_info.maxUv = XMFLOAT2(1.0f, 0.5f);
	view_info.minUv = XMFLOAT2(0.0f, 0.0f);
	view_info.positions.emplace_back(XMFLOAT3(-0.43f, 0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);
	view_info.positions.emplace_back(XMFLOAT3(-0.43f, -0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.5f);
	uiShader = new ImageShader;
	uiShader->BuildObjects(&view_info);
	instacingUiShaders.emplace_back(uiShader);
	view_info.f_uvY.clear();
	view_info.positions.clear();

	view_info.textureName = "Resources/Images/Time_Number.dds";
	view_info.meshSize = XMFLOAT2(0.06f, 0.085f);
	view_info.maxUv = XMFLOAT2(0.1f, 0.5f);
	view_info.minUv = XMFLOAT2(0.0f, 0.0f);

	view_info.positions.emplace_back(XMFLOAT3(-0.26f, -0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);
	view_info.positions.emplace_back(XMFLOAT3(-0.17f, -0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);
	view_info.positions.emplace_back(XMFLOAT3(-0.08, -0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);
	view_info.positions.emplace_back(XMFLOAT3(0.01f, -0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);
	view_info.positions.emplace_back(XMFLOAT3(0.10f, -0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);
	view_info.positions.emplace_back(XMFLOAT3(0.19f, -0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);
	view_info.positions.emplace_back(XMFLOAT3(0.28f, -0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);
	view_info.positions.emplace_back(XMFLOAT3(0.37f, -0.1f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(&view_info);
	uiShader->getUvXs()[2] = 10.0f;
	uiShader->getUvXs()[5] = 10.0f;

	uiShader->getUvXs()[0] = EventHandler::GetInstance()->m_iMinute / 10;
	uiShader->getUvXs()[1] = EventHandler::GetInstance()->m_iMinute % 10;

	uiShader->getUvXs()[3] = EventHandler::GetInstance()->m_fSecond / 10;
	uiShader->getUvXs()[4] = (int)EventHandler::GetInstance()->m_fSecond % 10;

	int n = (int)(EventHandler::GetInstance()->m_fSecond * 100) % 100;
	uiShader->getUvXs()[6] = n / 10;
	uiShader->getUvXs()[7] = n % 10;
	instancingNumberUiShaders.emplace_back(uiShader);


	gameTexts.emplace_back(GameText(XMFLOAT2(0.4f, 0.4f), XMFLOAT2(1.05f, 1.05f))); //winner name font
	gameTexts[0].text = EventHandler::GetInstance()->m_sWinner;
	CreateShaderVariables();

	SoundManager::GetInstance()->Play("End_BGM", 0.2f);
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
			//네트워크 클래스에 있는 방번호가 아직 있으므로 그 번호의 RoomScene으로 넘어가도록 한다.
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

void EndScene::Render()
{
	BaseScene::Render();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_BILLBOARD]);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	for (CUiShader* shader : instacingUiShaders)
	{
		if (shader)
			shader->Render(m_pd3dCommandList, m_pCamera);
	}
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI_NUMBER]);
	for (CUiShader* shader : instancingNumberUiShaders)
	{
		if (shader)
			shader->Render(m_pd3dCommandList, m_pCamera);
	}

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_FONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList, m_pCamera, gameTexts);

#ifdef _WITH_BOUND_BOX

#endif
}


void EndScene::AnimateObjects(float fTimeElapsed)
{
	//말 그대로 애니메이션 update

}

SceneType EndScene::Update(float fTimeElapsed)
{
	//물리 및 충돌을 위한 update
	if (sceneType != SceneType::End_Scene)
	{
		SoundManager::GetInstance()->AllStop();
		return sceneType;
	}

	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);
	return SceneType::End_Scene;
}


void EndScene::CreateShaderVariables()
{
}


void EndScene::UpdateShaderVariables()
{
}

void EndScene::ReleaseShaderVariables()
{

}


void EndScene::ProcessPacket(char* packet)
{

}