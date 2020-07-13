#include "BillBoardShader.h"
#include "../../Common/FrameWork/CreateManager.h"
#include "TerrainObject.h"
#include "BillBoardObject.h"
#include "BillBoardMesh.h"

BillBoardShader::BillBoardShader()
{
}
BillBoardShader::~BillBoardShader()
{
}

void BillBoardShader::Load(CreateManager* pCreateManager, const char* filename)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, filename, "rb");
	if (!pInFile)
		return;
	
	BillBoardObject *pBillBoardObject = NULL;
	UINT nReads;
	int nLength = 0;

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);

	BillBoardMesh *mesh = NULL;
	mesh = new BillBoardMesh();

	for (int i = 0; i < nLength; ++i)
	{
		pBillBoardObject = new BillBoardObject(1);
		pBillBoardObject->AddRef();
		nReads = (UINT)::fread(&(pBillBoardObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		pBillBoardObject->SetMesh(mesh);
		objectList.emplace_back(pBillBoardObject);
	}

	::fclose(pInFile);
}
void BillBoardShader::BuildObjects(CreateManager* pCreateManager, void* pInformation)
{
	MODEL_INFO* info = (MODEL_INFO*)pInformation;
	if (!info->modelName)
		return;
	if (info->updatedContext)
		m_pUpdatedContext = info->updatedContext;

	CTexture * billboard = new CTexture(1, RESOURCE_TEXTURE2DARRAY, 0);
	billboard->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(info->modelName), 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, billboard, 8, true);

	m_ppObjects = new BillBoardObject(1);
	m_ppObjects->AddRef();

	BillBoardObject *pBillBoardObject = NULL;
	CMaterial *material = new CMaterial(1);

	//material->m_xmf4AmbientColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	//material->m_xmf4DiffuseColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	material->SetTexture(billboard);
	material->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	BillBoardMesh *mesh = NULL;
	mesh = new BillBoardMesh();
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);
	if (info->dataFileName)
		Load(pCreateManager, info->dataFileName);
	CreateShaderVariables(pCreateManager);

	UINT ncbElementBytes = ((sizeof(CB_BillBoard) + 255) & ~255); //256의 배수
	m_pd3dcbStruct = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbStruct->Map(0, NULL, (void **)&billBoardCb);
	billBoardCb->fSize = info->size;
}
void BillBoardShader::BuildObjects(CreateManager* pCreateManager, float size, const char *pszFileName, const char* filename)
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

	//material->m_xmf4AmbientColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	//material->m_xmf4DiffuseColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

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

	UINT ncbElementBytes = ((sizeof(CB_BillBoard) + 255) & ~255); //256의 배수
	m_pd3dcbStruct = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbStruct->Map(0, NULL, (void **)&billBoardCb);
	billBoardCb->fSize = size;
}

void BillBoardShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, m_pd3dcbStruct->GetGPUVirtualAddress());
	CObInstancingShader::Render(pd3dCommandList, pCamera);
}
void BillBoardShader::ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, m_pd3dcbStruct->GetGPUVirtualAddress());
	CObInstancingShader::ShadowRender(pd3dCommandList, pCamera);
}