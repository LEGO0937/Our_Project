#pragma once
#include "../Shader.h"

class SkinedShader : public CSkinedObInstancingShader
{
public:
	SkinedShader();
	virtual ~SkinedShader();
	
	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
		
	virtual void Update(float fTimeElapsed) {}
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
};


class PlayerShader : public SkinedShader
{
public:
	PlayerShader();
	virtual ~PlayerShader();

	virtual void Update(float fTimeElapsed);
	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
};