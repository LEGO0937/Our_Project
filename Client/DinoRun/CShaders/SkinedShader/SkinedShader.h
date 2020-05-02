#pragma once
#include "../Shader.h"

class SkinedShader : public CSkinedObInstancingShader
{
public:
	SkinedShader();
	virtual ~SkinedShader();
	
	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation) {}
	virtual void BuildObjects(CreateManager* pCreateManager, const char *pszFileName, const char* filename = NULL);
	
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
};


class PlayerShader : public SkinedShader
{
public:
	PlayerShader();
	virtual ~PlayerShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
};