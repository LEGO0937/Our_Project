#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/TerrainObject.h"

ModelShader::ModelShader()
{
}
ModelShader::~ModelShader()
{
}

void ModelShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	CGameObject *pModelObject = NULL;
	UINT nReads;
	int nLength = 0;

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, filename, NULL);
		pModelObject = pModel->m_pModelRootObject;
		pModelObject->AddRef();
		nReads = (UINT)::fread(&(pModelObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pModelObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}

void ModelShader::BuildObjects(CreateManager* pCreateManager, const char *pszFileName, const char* filename)
{
	if (!pszFileName)
		return;

	CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager,  pszFileName, NULL);
	m_ppObjects = pModel->m_pModelRootObject;

	m_ppObjects->AddRef();

	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}

	if(filename)
		Load(pCreateManager, pszFileName, filename);

	CreateShaderVariables(pCreateManager);
}
