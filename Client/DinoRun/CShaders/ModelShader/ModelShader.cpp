#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/TerrainObject.h"
#include "../../Meshes/BillBoardMesh.h"

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
	//빌보드의 텍스처를 위한 쉐이더를 하나 더 만들고 텍스처 정보를 담은 쉐이더를 빌보드오브젝트가 갖도록 한다. 

	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}
	
	string sBillBoardName = pszFileName;
	instancingModelName = pszFileName;
	instancingModelName.insert(instancingModelName.find("."), "_ins");  //인스턴싱 전용 모델파일을 불러온다
											  //텍스처,메시의 정보들이 들어 있지않음.
	if(filename)
		Load(pCreateManager, pszFileName, filename);
	
	//sBillBoardName.insert(sBillBoardName.find("."), "_B");
	sBillBoardName.replace(sBillBoardName.find
		(".bin"),4 ,".dds");

	sBillBoardName.replace(sBillBoardName.find
	("M_"),2 ,"B_");
		
	
	/*
	CMaterial* pMaterial = new CMaterial(1);

	CShader* pShader = new CShader();
	pShader->CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	//string name = pszFileName.c_str();
	pTexture->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(pstrToken), 0);
	pShader->CreateShaderResourceViews(pCreateManager, pTexture, 8, true);
	pMaterial->SetTexture(pTexture);
	pMaterial->SetShader(pShader);
	*/

	//billBoardCB
	UINT ncbElementBytes = ((sizeof(CB_BillBoard) + 255) & ~255); //256의 배수
	m_pd3dcbStruct = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbStruct->Map(0, NULL, (void **)&billBoardCb);

	//billBoardCb->fSize = 6;

	//각 셰이더의 로드 함수에서 직접 빌보드 크기 정할 것.

	CreateShaderVariables(pCreateManager);
}

void ModelShader::BuildObjects(CreateManager* pCreateManager, float size, const char *pszFileName, const char* filename)
{
	if (!pszFileName)
		return;

	CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, pszFileName, NULL);
	m_ppObjects = pModel->m_pModelRootObject;

	m_ppObjects->AddRef();
	//빌보드의 텍스처를 위한 쉐이더를 하나 더 만들고 텍스처 정보를 담은 쉐이더를 빌보드오브젝트가 갖도록 한다. 

	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}

	string sBillBoardName = pszFileName;
	instancingModelName = pszFileName;
	instancingModelName.insert(instancingModelName.find("."), "_ins");  //인스턴싱 전용 모델파일을 불러온다
											  //텍스처,메시의 정보들이 들어 있지않음.

	//sBillBoardName.insert(sBillBoardName.find("."), "_B");
	sBillBoardName.replace(sBillBoardName.find
	(".bin"), 4, ".dds");

	sBillBoardName.replace(sBillBoardName.find
	("M_"), 2, "B_");
	sBillBoardName.replace(sBillBoardName.find
	("Models"), 6, "Images");
	if (size == 1)
	{  //빌보드 오브젝트 생성할것
		
		m_pBillBoardObject = new CGameObject(1);

		CMaterial* pMaterial = new CMaterial(1);
		
		CShader* pShader = new CShader();
		pShader->CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);
		CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
		//string name = pszFileName.c_str();
		pTexture->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(sBillBoardName.c_str()), 0);
		pShader->CreateShaderResourceViews(pCreateManager, pTexture, 8, true);
		pMaterial->SetTexture(pTexture);
		pMaterial->SetShader(pShader);
		pMaterial->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

		m_pBillBoardObject->SetMaterial(0, pMaterial);
		
		BillBoardMesh *mesh = NULL;
		mesh = new BillBoardMesh();
		mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

		m_pBillBoardObject->SetMesh(mesh);
		
		//billBoardCB
		UINT ncbElementBytes = ((sizeof(CB_BillBoard) + 255) & ~255); //256의 배수
		m_pd3dcbStruct = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
			ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_pd3dcbStruct->Map(0, NULL, (void **)&billBoardCb);

		//billBoardCb->fSize = 6;
	}
	//각 셰이더의 로드 함수에서 직접 빌보드 크기 정할 것.

	if (filename)
		Load(pCreateManager, pszFileName, filename);

	CreateShaderVariables(pCreateManager);

}