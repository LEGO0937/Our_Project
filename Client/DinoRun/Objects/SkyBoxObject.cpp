#include "SkyBoxObject.h"
#include "../Common/Camera/Camera.h"
#include "../Common/FrameWork/CreateManager.h"
SkyBoxObject::SkyBoxObject(shared_ptr<CreateManager> pCreateManager) : CGameObject(1)
{

	SkyBoxMesh *pSkyBoxMesh = new SkyBoxMesh(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), 2500.0f, 2500.0f, 2500.0f);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables(pCreateManager);

	CTexture *pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0);
	pSkyBoxTexture->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), L"Resources/Images/grasscube1024.dds", 0);

	SkyBoxShader *pSkyBoxShader = new SkyBoxShader();
	pSkyBoxShader->CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);
	pSkyBoxShader->CreateShaderResourceViews(pCreateManager, pSkyBoxTexture, 8, true);

	
	CMaterial *pSkyBoxMaterial = new CMaterial(1);
	pSkyBoxMaterial->m_xmf4AmbientColor = XMFLOAT4(0.3, 0.3, 0.3, 1.0);
	pSkyBoxMaterial->m_xmf4DiffuseColor = XMFLOAT4(0.5, 0.7, 0.5, 1.0);

	pSkyBoxMaterial->SetTexture(pSkyBoxTexture,0);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	pSkyBoxMaterial->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	SetMaterial(0, pSkyBoxMaterial);

}

SkyBoxObject::~SkyBoxObject()
{
}

void SkyBoxObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	CGameObject::Render(pd3dCommandList, pCamera);
}