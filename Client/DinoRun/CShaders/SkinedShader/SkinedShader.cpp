#include "SkinedShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/TerrainObject.h"

#include "../../Common/Animation/Animation.h"

SkinedShader::SkinedShader()
{
}
SkinedShader::~SkinedShader()
{
}
void SkinedShader::Load(shared_ptr<CreateManager> pCreateManager, const char* filename, const char* Loadname)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	CGameObject *pSkinedObject = NULL;
	UINT nReads;
	int nLength = 0;

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, filename, NULL);
		pSkinedObject = pModel->m_pModelRootObject;
		pSkinedObject->AddRef();

		pSkinedObject->m_pSkinnedAnimationController = new CAnimationController(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), 1, pModel);
		pSkinedObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0); //left_turn_start
		pSkinedObject->m_pSkinnedAnimationController->SetTrackEnable(0, false);

		nReads = (UINT)::fread(&(pSkinedObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pSkinedObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}

void SkinedShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, const char *pszFileName, const char* filename)
{
	if (!pszFileName)
		return;

	CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, pszFileName, NULL);
	m_ppObjects = pModel->m_pModelRootObject;

	m_ppObjects->m_pSkinnedAnimationController = new CAnimationController(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), 1, pModel);
	m_ppObjects->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0); //left_turn_start
	m_ppObjects->m_pSkinnedAnimationController->SetTrackEnable(0, false);

	m_ppObjects->AddRef();

	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}

	if (filename)
		Load(pCreateManager, pszFileName, filename);

	CreateShaderVariables(pCreateManager);
}

void SkinedShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
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