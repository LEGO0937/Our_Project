#include "MinimapShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "TerrainObject.h"
#include "BillBoardObject.h"
#include "LiePlaneMesh.h"

MinimapShader::MinimapShader()
{
}
MinimapShader::~MinimapShader()
{
}

void MinimapShader::BuildObjects(const char *pszFileName, const char* filename)
{
	if (!pszFileName)
		return;

	CTexture * miniMap = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	miniMap->LoadTextureFromFile(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get(), ConvertCHARtoWCHAR(pszFileName), 0);

	CreateCbvSrvDescriptorHeaps(0, 1);

	CreateShaderResourceViews(miniMap, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->m_xmf4AmbientColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	material->m_xmf4DiffuseColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	material->SetTexture(miniMap);
	material->CreateShaderVariable(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	LiePlaneMesh *mesh = NULL;
	mesh = new LiePlaneMesh(257.0*12.5, 1.0, 257.0 * 12.5, 0.0, 1.0, 0.0, 1.0);
	mesh->CreateShaderVariables(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);


	pObject = new CGameObject(1);
	pObject->SetPosition(257.0*12.5, 100, 257.0*12.5);
	pObject->SetMesh(mesh);
	objectList.emplace_back(pObject);

	CreateShaderVariables();
}
