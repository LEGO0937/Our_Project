#include "ModelShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "CheckPointObject.h"

BlockShader::BlockShader()
{
	shaderName = _BLOCK_SHADER;
}
BlockShader::~BlockShader()
{

}

void BlockShader::Load(const char* filename, const char* Loadname)
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
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(fileName, NULL);
		
		pBlockObject = new CheckPointObject;
		pBlockObject->SetChild(pModel->m_pModelRootObject->GetChild());
		pBlockObject->AddRef();

		//�̰����� findFrame�� ���� �� ������Ʈ�� ���� �� Ű�׸�ƽ �� �߰��� ��.
		nReads = (UINT)::fread(&(pBlockObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		pBlockObject->UpdateTransform(NULL);
		pBlockObject->SetName(shaderName + to_string(m_iCurSerealNum));
		pBlockObject->SetId(m_iCurSerealNum++);
		objectList.emplace_back(pBlockObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
	//o

}