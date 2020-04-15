#include "FenceShader.h"
#include "../../Objects/TerrainObject.h"
#include "../../Common/ImGui/imgui/imgui.h"

RectShader::RectShader()
{
}
RectShader::~RectShader()
{
}


void RectShader::Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix)
{
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Rect.bin", NULL);
	pAngrybotModel->m_pModelRootObject->AddRef();
	pAngrybotModel->m_pModelRootObject->m_xmf4x4ToParent = matrix;
	objectList.emplace_back(pAngrybotModel->m_pModelRootObject);
}
void RectShader::Delete()
{
	if (objectList.size() <= 1)
		return;
	objectList.pop_back();
}


void RectShader::Save()
{
	if (objectList.size() <= 1)
		return;

	FILE *pOutFile = NULL;
	::fopen_s(&pOutFile, "RectData", "wb");

	int n = 0;
	UINT nReads;
	int nLength = objectList.size() - 1;
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
void RectShader::Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, "Resources/ObjectData/RectData(LineBox)", "rb");
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

	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Rect.bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	objectList.emplace_back(pTreeObject);

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Rect.bin", NULL);
		pTreeObject = pAngrybotModel->m_pModelRootObject;
		pTreeObject->AddRef();
		nReads = (UINT)::fread(&(pTreeObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pTreeObject);
	}

	::fclose(pInFile);
}

void RectShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain)
{
	int i = 0;
	m_nObjects = 1;
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Rect.bin", NULL);
	m_ppObjects = pAngrybotModel->m_pModelRootObject;

	m_ppObjects->AddRef();

	if (pAngrybotModel)
	{
		delete pAngrybotModel;
		pAngrybotModel = NULL;
	}
	CGameObject *pTreeObject = NULL;

	CHeightMapTerrain* Terrain = (CHeightMapTerrain*)terrain;

	pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Rect.bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	pTreeObject->SetPosition(200, 0, 0);

	objectList.emplace_back(pTreeObject);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void RectShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
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



//--------------

MFenceShader::MFenceShader()
{
}
MFenceShader::~MFenceShader()
{
}


void MFenceShader::Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix)
{
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.3x0.3).bin", NULL);
	pAngrybotModel->m_pModelRootObject->AddRef();
	pAngrybotModel->m_pModelRootObject->m_xmf4x4ToParent = matrix;
	objectList.emplace_back(pAngrybotModel->m_pModelRootObject);
}
void MFenceShader::Delete()
{
	if (objectList.size() <= 1)
		return;
	objectList.pop_back();
}


void MFenceShader::Save()
{
	if (objectList.size() <= 1)
		return;

	FILE *pOutFile = NULL;
	::fopen_s(&pOutFile, "MFenceData", "wb");

	int n = 0;
	UINT nReads;
	int nLength = objectList.size() - 1;
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
void MFenceShader::Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, "Resources/ObjectData/MFenceData", "rb");
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

	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.3x0.3).bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	objectList.emplace_back(pTreeObject);

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.3x0.3).bin", NULL);
		pTreeObject = pAngrybotModel->m_pModelRootObject;
		pTreeObject->AddRef();
		nReads = (UINT)::fread(&(pTreeObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pTreeObject);
	}

	::fclose(pInFile);
}

void MFenceShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain)
{

	int i = 0;
	m_nObjects = 1;
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.3x0.3).bin", NULL);
	m_ppObjects = pAngrybotModel->m_pModelRootObject;

	m_ppObjects->AddRef();

	if (pAngrybotModel)
	{
		delete pAngrybotModel;
		pAngrybotModel = NULL;
	}
	CGameObject *pTreeObject = NULL;

	CHeightMapTerrain* Terrain = (CHeightMapTerrain*)terrain;

	pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.3x0.3).bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	pTreeObject->SetPosition(200, 0, 0);

	objectList.emplace_back(pTreeObject);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void MFenceShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
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
//--------------------------

LFenceShader::LFenceShader()
{
}
LFenceShader::~LFenceShader()
{
}


void LFenceShader::Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix)
{
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.9x0.3).bin", NULL);
	pAngrybotModel->m_pModelRootObject->AddRef();
	pAngrybotModel->m_pModelRootObject->m_xmf4x4ToParent = matrix;
	objectList.emplace_back(pAngrybotModel->m_pModelRootObject);
}
void LFenceShader::Delete()
{
	if (objectList.size() <= 1)
		return;
	objectList.pop_back();
}


void LFenceShader::Save()
{
	if (objectList.size() <= 1)
		return;

	FILE *pOutFile = NULL;
	::fopen_s(&pOutFile, "LFenceData", "wb");

	int n = 0;
	UINT nReads;
	int nLength = objectList.size() - 1;
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
void LFenceShader::Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, "Resources/ObjectData/LFenceData", "rb");
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

	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.9x0.3).bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	objectList.emplace_back(pTreeObject);

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.9x0.3).bin", NULL);
		pTreeObject = pAngrybotModel->m_pModelRootObject;
		pTreeObject->AddRef();
		nReads = (UINT)::fread(&(pTreeObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pTreeObject);
	}

	::fclose(pInFile);
}

void LFenceShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain)
{
	int i = 0;
	m_nObjects = 1;
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.9x0.3).bin", NULL);
	m_ppObjects = pAngrybotModel->m_pModelRootObject;

	m_ppObjects->AddRef();

	if (pAngrybotModel)
	{
		delete pAngrybotModel;
		pAngrybotModel = NULL;
	}
	CGameObject *pTreeObject = NULL;

	CHeightMapTerrain* Terrain = (CHeightMapTerrain*)terrain;

	pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Fence(0.9x0.3).bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	pTreeObject->SetPosition(200, 0, 0);

	objectList.emplace_back(pTreeObject);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void LFenceShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
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

//-----------------------------

MeatShader::MeatShader()
{
}
MeatShader::~MeatShader()
{
}


void MeatShader::Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix)
{
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Meat.bin", NULL);
	pAngrybotModel->m_pModelRootObject->AddRef();
	pAngrybotModel->m_pModelRootObject->m_xmf4x4ToParent = matrix;
	objectList.emplace_back(pAngrybotModel->m_pModelRootObject);
}
void MeatShader::Delete()
{
	if (objectList.size() <= 1)
		return;
	objectList.pop_back();
}


void MeatShader::Save()
{
	if (objectList.size() <= 1)
		return;

	FILE *pOutFile = NULL;
	::fopen_s(&pOutFile, "MeatData", "wb");

	int n = 0;
	UINT nReads;
	int nLength = objectList.size() - 1;
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
void MeatShader::Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, "Resources/ObjectData/MeatData", "rb");
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

	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Meat.bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	objectList.emplace_back(pTreeObject);

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Meat.bin", NULL);
		pTreeObject = pAngrybotModel->m_pModelRootObject;
		pTreeObject->AddRef();
		nReads = (UINT)::fread(&(pTreeObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pTreeObject);
	}

	::fclose(pInFile);
}

void MeatShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain)
{
	int i = 0;
	m_nObjects = 1;
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Meat.bin", NULL);
	m_ppObjects = pAngrybotModel->m_pModelRootObject;

	m_ppObjects->AddRef();

	if (pAngrybotModel)
	{
		delete pAngrybotModel;
		pAngrybotModel = NULL;
	}
	CGameObject *pTreeObject = NULL;

	CHeightMapTerrain* Terrain = (CHeightMapTerrain*)terrain;

	pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/Meat.bin", NULL);
	pTreeObject = pAngrybotModel->m_pModelRootObject;
	pTreeObject->AddRef();
	pTreeObject->SetPosition(200, 0, 0);

	objectList.emplace_back(pTreeObject);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void MeatShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
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