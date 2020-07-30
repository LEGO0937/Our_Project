#include "TerrainShader.h"

TerrainShader::TerrainShader()
{
}
TerrainShader::~TerrainShader()
{

}

void TerrainShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
}