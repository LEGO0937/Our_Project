#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/CheckPointObject.h"

BlockShader::BlockShader()
{
}
BlockShader::~BlockShader()
{
}

void BlockShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	isEnable = false;
	FILE *pInFile = NULL;

	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	CheckPointObject *pBlockObject = NULL;
	UINT nReads;
	int nLength = 0;

	const char* fileName = instancingModelName.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		
		pBlockObject = new CheckPointObject;
		pBlockObject->SetChild(pModel->m_pModelRootObject->m_pChild);
		pBlockObject->AddRef();

		pBlockObject->UpdateTransform(NULL);
		//�̰����� findFrame�� ���� �� ������Ʈ�� ���� �� Ű�׸�ƽ �� �߰��� ��.
		nReads = (UINT)::fread(&(pBlockObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pBlockObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}