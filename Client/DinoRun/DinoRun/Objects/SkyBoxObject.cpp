#include "SkyBoxObject.h"
#include "../Common/Camera/Camera.h"

#include "../Common/FrameWork/GameManager.h"
SkyBoxObject::SkyBoxObject() : CGameObject(1)
{

	SkyBoxMesh *pSkyBoxMesh = new SkyBoxMesh(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get(), 2500.0f, 2500.0f, 2500.0f);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables();

	CTexture *pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0);
	pSkyBoxTexture->LoadTextureFromFile(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get(), L"Resources/Images/grasscube1024.dds", 0);

	SkyBoxShader *pSkyBoxShader = new SkyBoxShader();
	pSkyBoxShader->CreateCbvSrvDescriptorHeaps(0, 1);
	pSkyBoxShader->CreateShaderResourceViews(pSkyBoxTexture, 8, true);

	
	CMaterial *pSkyBoxMaterial = new CMaterial(1);
	pSkyBoxMaterial->m_xmf4AmbientColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	pSkyBoxMaterial->m_xmf4DiffuseColor = XMFLOAT4(0.5f, 0.7f, 0.5f, 1.0f);

	pSkyBoxMaterial->SetTexture(pSkyBoxTexture,0);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	pSkyBoxMaterial->CreateShaderVariable(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());
	SetMaterial(0, pSkyBoxMaterial);

	CreateBuffer();
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