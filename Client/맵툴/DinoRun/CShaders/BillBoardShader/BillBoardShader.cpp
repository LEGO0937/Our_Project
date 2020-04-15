#include "BillBoardShader.h"
#include "../../Objects/TerrainObject.h"
#include "../../Objects/BillBoardObject.h"
#include "../../Meshes/BillBoardMesh.h"
#include "../../Common/ImGui/imgui/imgui.h"

BillBoardShader::BillBoardShader()
{
}
BillBoardShader::~BillBoardShader()
{
}


void BillBoardShader::Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix)
{
	BillBoardObject *pBillBoardObject = NULL;
	pBillBoardObject = new BillBoardObject(1);
	pBillBoardObject->AddRef();
	pBillBoardObject->m_xmf4x4ToParent = matrix;
	objectList.emplace_back(pBillBoardObject);
	
	//ReleaseShaderVariables();
	//CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void BillBoardShader::Delete()
{
	if (objectList.size() <= 1)
		return;
	objectList.pop_back();
}
void BillBoardShader::Save()
{
	if (objectList.size() <= 1)
		return;

	FILE *pOutFile = NULL;
	::fopen_s(&pOutFile, "BillBoardData", "wb");

	int n = 0;
	UINT nReads;
	int nLength = objectList.size()-1;
	::fwrite(&nLength, sizeof(int), 1, pOutFile);

	for (CGameObject* ob : objectList)
	{
		if (n == 0)
		{
			n++;
			continue;
		}
		nReads = (UINT)::fwrite(&(ob->m_xmf4x4ToParent), sizeof(XMFLOAT4X4),1, pOutFile);
	}
	::fclose(pOutFile);
}
void BillBoardShader::Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, "Resources/ObjectData/BillBoardData", "rb");
	if (!pInFile)
		return;
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Release();
		}
		objectList.clear();
	}

	BillBoardObject *pBillBoardObject = NULL;
	UINT nReads;
	int nLength =0;

	pBillBoardObject = new BillBoardObject(1);
	pBillBoardObject->AddRef();
	objectList.emplace_back(pBillBoardObject);

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
void BillBoardShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain)
{
	CTexture * billboard = new CTexture(1, RESOURCE_TEXTURE2DARRAY, 0);
	billboard->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Resources/Images/treearray.dds", 0);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 1);

	CreateShaderResourceViews(pd3dDevice,  billboard, 8, true);

	int i = 0;
	m_nObjects = 1;
	m_ppObjects = new BillBoardObject(1);
	m_ppObjects->AddRef();
	
	BillBoardObject *pBillBoardObject = NULL;

	CHeightMapTerrain* Terrain = (CHeightMapTerrain*)terrain;

	CMaterial *material = new CMaterial(1);
	
	material->m_xmf4AmbientColor = XMFLOAT4(0.9, 0.9, 0.9, 1);
	material->m_xmf4DiffuseColor = XMFLOAT4(0.7, 0.7, 0.7, 1);

	material->SetTexture(billboard);
	material->CreateShaderVariable(pd3dDevice, pd3dCommandList);
	m_ppObjects->SetMaterial(0, material);


	pBillBoardObject = new BillBoardObject(1);
	pBillBoardObject->AddRef();
	pBillBoardObject->SetPosition(200, 0, 0);

	objectList.emplace_back(pBillBoardObject);
	


	//인스턴싱을 사용하여 렌더링하기 위하여 하나의 게임 객체만 메쉬를 가진다.

	BillBoardMesh *mesh = NULL;
	mesh = new BillBoardMesh();
	mesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//m_ppObjects->SetMaterial(0, material);
	m_ppObjects->SetMesh(mesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void BillBoardShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	
	if (objectList.size() > 0)
	{
		CShader::Render(pd3dCommandList, pCamera);
		//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 
		UpdateShaderVariables(pd3dCommandList);
		//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
		m_ppObjects->Render(pd3dCommandList, pCamera, objectList.size());
	}
}