#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"

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
	CGameObject *pFenceObject = NULL;
	UINT nReads;
	int nLength = 0;

	string string = filename;
	string.insert(string.find("."), "_ins");
	const char* fileName = string.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pFenceObject = pModel->m_pModelRootObject;
		pFenceObject->AddRef();
		pFenceObject->m_fMass = 200;
		pFenceObject->isKinematic = true;
		pFenceObject->m_ModelType = ModelType::Fence;
		//�̰����� findFrame�� ���� �� ������Ʈ�� ���� �� Ű�׸�ƽ �� �߰��� ��.
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