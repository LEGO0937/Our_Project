#include "ModelShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "FenceObject.h"
FenceShader::FenceShader()
{
	shaderName = _FENCE_SHADER;
}
FenceShader::~FenceShader()
{

}

void FenceShader::Load(const char* filename, const char* Loadname)
{
	isEnable = false;

	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	FenceObject *pFenceObject = NULL;
	UINT nReads;
	int nLength = 0;

	const char* fileName = instancingModelName.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(fileName, NULL);
		pFenceObject = new FenceObject;
		pFenceObject->SetChild(pModel->m_pModelRootObject->GetChild());
		pFenceObject->AddRef();
		//이곳에서 findFrame을 통해 각 오브젝트에 질량 및 키네마틱 값 추가할 것.
		nReads = (UINT)::fread(&(pFenceObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		pFenceObject->SetName(shaderName + to_string(m_iCurSerealNum));
		pFenceObject->SetId(m_iCurSerealNum++);
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