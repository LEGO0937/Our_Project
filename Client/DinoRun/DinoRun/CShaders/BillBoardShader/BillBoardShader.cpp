#include "BillBoardShader.h"
#include "../../Common/FrameWork/CreateManager.h"
#include "../../Objects/TerrainObject.h"
#include "../../Objects/BillBoardObject.h"
#include "../../Meshes/BillBoardMesh.h"

BillBoardShader::BillBoardShader()
{
}
BillBoardShader::~BillBoardShader()
{
}

void BillBoardShader::Load(shared_ptr<CreateManager> pCreateManager, const char* filename)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, filename, "rb");
	if (!pInFile)
		return;
	
	BillBoardObject *pBillBoardObject = NULL;
	UINT nReads;
	int nLength = 0;

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		pBillBoardObject = new BillBoardObject(1);
		pBillBoardObject->AddRef();
		nReads = (UINT)::fread(&(pBillBoardObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pBillBoardObject);
	}

	::fclose(pInFile);
}

void BillBoardShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, const char *pszFileName, const char* filename)
{
	if (!pszFileName)
		return;


	CTexture * billboard = new CTexture(1, RESOURCE_TEXTURE2DARRAY, 0);
	billboard->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(pszFileName), 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, billboard, 8, true);

	m_ppObjects = new BillBoardObject(1);
	m_ppObjects->AddRef();

	BillBoardObject *pBillBoardObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->m_xmf4AmbientColor = XMFLOAT4(0.7, 0.7, 0.7, 1);
	material->m_xmf4DiffuseColor = XMFLOAT4(0.3, 0.3, 0.3, 1);

	material->SetTexture(billboard);
	material->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	BillBoardMesh *mesh = NULL;
	mesh = new BillBoardMesh();
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);
	if (filename)
		Load(pCreateManager, filename);
	CreateShaderVariables(pCreateManager);
}
