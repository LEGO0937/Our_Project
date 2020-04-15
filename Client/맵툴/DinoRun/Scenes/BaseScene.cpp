#include "../Common/stdafx.h"
#include "BaseScene.h"
#include "../Objects/PlayerObject.h"
#include "../Objects/SkyBoxObject.h"
#include "../Common/Camera/Camera.h"
BaseScene::BaseScene()
{
	m_pd3dGraphicsRootSignature = NULL;
}

void BaseScene::ReleaseObjects()
{
	//for (CObjectsShader* shader : shaders)
	//{
	//	shader->ReleaseShaderVariables();
	//	shader->ReleaseObjects();
	//	if (shader) delete[] shader;
	//
	//}
	if (m_pSkyBox)
	{
		m_pSkyBox->Release();
		m_pSkyBox = NULL;
	}
	if (m_pLights) delete m_pLights;
	//terrain 포함
}

void BaseScene::AnimateObjects(float fTimeElapsed)
{

	//m_pPlayer->Animate(fTimeElapsed, NULL);
	//m_pPlayer->UpdateTransform(NULL);

}

void BaseScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);


	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress =
		m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(5, d3dcbLightsGpuVirtualAddress);

}

void BaseScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

}

void BaseScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
}
void BaseScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}
BaseScene::~BaseScene()
{
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();
}


void BaseScene::ReleaseUploadBuffers()
{

	//if (m_pTerrain)
	//	m_pTerrain->ReleaseUploadBuffers();
	
	//bulletShader->ReleaseUploadBuffers();
	if (m_pSkyBox)
		m_pSkyBox->ReleaseUploadBuffers();
}

void BaseScene::SetGraphicsRootSignature(ID3D12RootSignature *pd3dRootSignature)
{
	m_pd3dGraphicsRootSignature = pd3dRootSignature;
	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->AddRef();
}

void BaseScene::SetPipelineStates(int nPipelineStates, ID3D12PipelineState** ppd3dPipelineStates)
{
	m_nPipelineStates = nPipelineStates;
	m_ppd3dPipelineStates = ppd3dPipelineStates;
}

void BaseScene::setPlayer(CPlayer* player)
{
	m_pPlayer = player;
	m_pPlayer->SetPlayerUpdatedContext((CHeightMapTerrain*)m_pTerrain);
}

void BaseScene::setCamera(CCamera* camera)
{
	m_pCamera = camera;
	m_pPlayer->SetCameraUpdatedContext((CHeightMapTerrain*)m_pTerrain);
}