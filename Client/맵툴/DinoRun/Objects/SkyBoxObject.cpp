#include "SkyBoxObject.h"
#include "../Common/Camera/Camera.h"

SkyBoxObject::SkyBoxObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature) : CGameObject(1)
{
	SkyBoxMesh *pSkyBoxMesh = new SkyBoxMesh(pd3dDevice, pd3dCommandList, 40.0f, 40.0f, 40.0f);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Resources/Images/grasscube1024.dds", 0);

	SkyBoxShader *pSkyBoxShader = new SkyBoxShader();
	pSkyBoxShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 1);
	pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 8, true);

	
	CMaterial *pSkyBoxMaterial = new CMaterial(1);
	pSkyBoxMaterial->m_xmf4AmbientColor = XMFLOAT4(0.3, 0.3, 0.3, 1.0);
	pSkyBoxMaterial->m_xmf4DiffuseColor = XMFLOAT4(0.5, 0.7, 0.5, 1.0);

	pSkyBoxMaterial->SetTexture(pSkyBoxTexture,0);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	pSkyBoxMaterial->CreateShaderVariable(pd3dDevice, pd3dCommandList);
	SetMaterial(0, pSkyBoxMaterial);

}

SkyBoxObject::~SkyBoxObject()
{
}

void SkyBoxObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	//SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	CGameObject::Render(pd3dCommandList, pCamera);
}