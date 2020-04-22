#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/FenceObject.h"
FenceShader::FenceShader()
{
}
FenceShader::~FenceShader()
{
}

void FenceShader::Load(shared_ptr<CreateManager> pCreateManager, const char* filename, const char* Loadname)
{
	isEnable = false;

	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	FenceObject *pFenceObject = NULL;
	UINT nReads;
	int nLength = 0;

	string string = filename;
	string.insert(string.find("."), "_ins");
	const char* fileName = string.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pFenceObject = new FenceObject;
		pFenceObject->SetChild(pModel->m_pModelRootObject->m_pChild);
		pFenceObject->AddRef();
		//이곳에서 findFrame을 통해 각 오브젝트에 질량 및 키네마틱 값 추가할 것.
		nReads = (UINT)::fread(&(pFenceObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pFenceObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}

void FenceShader::Update(float fTimeElapsed)
{
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Update(fTimeElapsed,NULL);
		}
	}
}