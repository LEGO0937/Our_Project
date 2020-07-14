#pragma once
#include "../Shader.h"

class TerrainShader : public CShader
{

public:
	TerrainShader();
	~TerrainShader();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};
