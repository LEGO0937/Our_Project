#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/ItemObject.h"

BananaShader::BananaShader()
{
}
BananaShader::~BananaShader()
{
}

void BananaShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	isEnable = true;

	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	ItemBanana* pItemObject = NULL;
	UINT nReads;
	int nLength = 0;

	const char* fileName = instancingModelName.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pItemObject = new ItemBanana;
		pItemObject->SetChild(pModel->m_pModelRootObject->m_pChild);
		pItemObject->AddRef();

		//이곳에서 findFrame을 통해 각 오브젝트에 질량 및 키네마틱 값 추가할 것.
		nReads = (UINT)::fread(&(pItemObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pItemObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}

void BananaShader::Update(float fTimeElapsed)
{
	if (objectList.size())
	{
		for (vector<CGameObject*>::iterator it = objectList.begin(); it != objectList.end();)
		{
			if ((*it)->Update(fTimeElapsed, NULL))
			{
				(*it)->Release();
				it = objectList.erase(it);
			}
			else
				it++;
		}
	}
}

void BananaShader::addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3Position)
{
	ItemBanana* pItemObject = NULL;
	CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, instancingModelName.c_str(), NULL);
	
	pItemObject = new ItemBanana;
	pItemObject->SetChild(pModel->m_pModelRootObject->m_pChild);
	pItemObject->AddRef();

	pItemObject->SetMatrix(xmf3Position);

	objectList.emplace_back(pItemObject);
	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}
	ReleaseShaderVariables();
	CreateShaderVariables(pCreateManager);
}