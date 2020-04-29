#include "MinimapShader.h"
#include "../../Common/FrameWork/CreateManager.h"
#include "../../Objects/TerrainObject.h"
#include "../../Objects/BillBoardObject.h"
#include "../../Meshes/LiePlaneMesh.h"

MinimapShader::MinimapShader()
{
}
MinimapShader::~MinimapShader()
{
}

void MinimapShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, const char *pszFileName, const char* filename)
{
	if (!pszFileName)
		return;

	CTexture * miniMap = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	miniMap->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(pszFileName), 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, miniMap, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->m_xmf4AmbientColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	material->m_xmf4DiffuseColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	material->SetTexture(miniMap);
	material->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	LiePlaneMesh *mesh = NULL;
	mesh = new LiePlaneMesh(257.0*12.5, 1.0, 257.0 * 12.5, 0.0, 1.0, 0.0, 1.0);
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);


	pObject = new CGameObject(1);
	pObject->SetPosition(257.0*12.5, 100, 257.0*12.5);
	objectList.emplace_back(pObject);

	CreateShaderVariables(pCreateManager);
}
