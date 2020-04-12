#pragma once
#include "BaseObject.h"
#include "../CShaders/SkyBoxShader/SkyBoxShader.h"
#include "../Meshes/SkyBoxMesh.h"

class SkyBoxObject : public CGameObject
{
public:
	SkyBoxObject(shared_ptr<CreateManager> pCreateManager);
	virtual ~SkyBoxObject();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};