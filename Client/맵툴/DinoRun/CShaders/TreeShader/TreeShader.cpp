#include "TreeShader.h"
#include "../../Objects/TerrainObject.h"
#include "../../Objects/BillBoardObject.h"
#include "../../Meshes/BillBoardMesh.h"
#include "../../Common/ImGui/imgui/imgui.h"


TreeShader::TreeShader()
{
}
TreeShader::~TreeShader()
{
}


void TreeShader::Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix)
{
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/tree.bin", NULL);
	pAngrybotModel->m_pModelRootObject->AddRef();
	pAngrybotModel->m_pModelRootObject->m_xmf4x4ToParent = matrix;
	objectList.emplace_back(pAngrybotModel->m_pModelRootObject);
	ReleaseShaderVariables();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void TreeShader::Delete()
{
	if (objectList.size() <= 1)
		return;
	objectList.pop_back();
}


void TreeShader::Save()
{
	if (objectList.size() <= 1)
		return;

	FILE *pOutFile = NULL;
	::fopen_s(&pOutFile, "TreeData", "wb");

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
		nReads = (UINT)::fwrite(&(ob->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pOutFile);
	}
	::fclose(pOutFile);
}
void TreeShader::Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, "Resources/ObjectData/TreeData", "rb");
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

	CGameObject *pTreeObject = NULL;

	UINT nReads;
	int nLength = 0;

	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/tree.bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	objectList.emplace_back(pTreeObject);

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/tree.bin", NULL);
		pTreeObject = pAngrybotModel->m_pModelRootObject;
		pTreeObject->AddRef();
		nReads = (UINT)::fread(&(pTreeObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pTreeObject);
	}

	::fclose(pInFile);
}

void TreeShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList,ID3D12RootSignature* pd3dGraphicsRootSignature,void* terrain)
{
	//CTexture * billboard = new CTexture(1, RESOURCE_TEXTURE2DARRAY, 0);
	//billboard->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Resources/Images/treearray.dds", 0);

	//CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 1);
	//
	//CreateShaderResourceViews(pd3dDevice, billboard, 8, true);

	int i = 0;
	m_nObjects = 1;
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/tree.bin", NULL);
	m_ppObjects = pAngrybotModel->m_pModelRootObject;

	m_ppObjects->AddRef();

	if (pAngrybotModel)
	{
		delete pAngrybotModel;
		pAngrybotModel = NULL;
	}
	CGameObject *pTreeObject = NULL;

	CHeightMapTerrain* Terrain = (CHeightMapTerrain*)terrain;

	//CMaterial *material = new CMaterial(1);

	//material->m_xmf4AmbientColor = XMFLOAT4(0.9, 0.9, 0.9, 1);
	//material->m_xmf4DiffuseColor = XMFLOAT4(0.7, 0.7, 0.7, 1);

	//material->SetTexture(billboard);
	//material->CreateShaderVariable(pd3dDevice, pd3dCommandList);
	//m_ppObjects->SetMaterial(0, material);

	pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/tree.bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	pTreeObject->SetPosition(200, 0, 0);

	objectList.emplace_back(pTreeObject);



	//인스턴싱을 사용하여 렌더링하기 위하여 하나의 게임 객체만 메쉬를 가진다.

	//BillBoardMesh *mesh = NULL;
	//mesh = new BillBoardMesh();
	//mesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//m_ppObjects->SetMaterial(0, material);
	//m_ppObjects->SetMesh(mesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void TreeShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
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