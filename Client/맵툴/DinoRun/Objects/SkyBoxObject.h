#pragma once
#include "BaseObject.h"
#include "../CShaders/SkyBoxShader/SkyBoxShader.h"
#include "../Meshes/SkyBoxMesh.h"

class SkyBoxObject : public CGameObject
{
public:
	SkyBoxObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~SkyBoxObject();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};