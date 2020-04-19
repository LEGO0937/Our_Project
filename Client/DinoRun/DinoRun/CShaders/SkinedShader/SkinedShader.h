#pragma once
#include "../Shader.h"

class SkinedShader : public CSkinedObInstancingShader
{
public:
	SkinedShader();
	virtual ~SkinedShader();
	
	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain) {}
	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, const char *pszFileName, const char* filename = NULL);
	
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Load(shared_ptr<CreateManager> pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
};


class PlayerShader : public SkinedShader
{
public:
	PlayerShader();
	virtual ~PlayerShader();

	virtual void Load(shared_ptr<CreateManager> pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
};