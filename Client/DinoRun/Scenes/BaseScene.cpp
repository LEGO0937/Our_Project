#include "../Common/stdafx.h"
#include "BaseScene.h"
#include "../Common/FrameWork/CreateManager.h"
#include "../Objects/PlayerObject.h"
#include "../Objects/SkyBoxObject.h"
#include "../Common/Camera/Camera.h"

BaseScene::BaseScene()
{
}

void BaseScene::ReleaseObjects()
{
	if (m_pLights) delete m_pLights;
}

void BaseScene::SetViewportsAndScissorRects()
{
	if (m_pCamera) m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList.Get());
}

void BaseScene::Render(float fTimeElapsed)
{
	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList.Get());
	m_pCamera->UpdateShaderVariables(m_pd3dCommandList.Get());

	UpdateShaderVariables();
	if (m_pd3dcbLights)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress =
			m_pd3dcbLights->GetGPUVirtualAddress();
		m_pd3dCommandList->SetGraphicsRootConstantBufferView(5, d3dcbLightsGpuVirtualAddress);
	}
}

void BaseScene::RenderShadow()
{
	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList.Get());
	UpdateShaderVariables();
}

BaseScene::~BaseScene()
{
}

void BaseScene::ReleaseUploadBuffers()
{
}

void BaseScene::SetGraphicsRootSignature(ID3D12RootSignature *pd3dRootSignature)
{
	m_pd3dGraphicsRootSignature = pd3dRootSignature;
}

void BaseScene::SetPipelineStates(int nPipelineStates, ID3D12PipelineState** ppd3dPipelineStates)
{
	m_nPipelineStates = nPipelineStates;
	m_ppd3dPipelineStates = ppd3dPipelineStates;
}

void BaseScene::setPlayer(CPlayer* player)
{
	m_pPlayer = player;
}

void BaseScene::setCamera(CCamera* camera)
{
	m_pCamera = camera;
}