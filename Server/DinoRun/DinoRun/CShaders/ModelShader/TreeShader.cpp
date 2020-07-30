#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"

TreeShader::TreeShader()
{
	shaderName = _TREE_SHADER;
}
TreeShader::~TreeShader()
{

}

void TreeShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	if (m_pd3dcbStruct)
		billBoardCb->fSize = 50;

	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	CGameObject *pTreeObject = NULL;
	UINT nReads;
	int nLength = 0;

	const char* fileName = instancingModelName.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pTreeObject = pModel->m_pModelRootObject;
		pTreeObject->AddRef();
		
		//이곳에서 findFrame을 통해 각 오브젝트에 질량 및 키네마틱 값 추가할 것.
		nReads = (UINT)::fread(&(pTreeObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		pTreeObject->UpdateTransform(NULL);
		pTreeObject->SetName(shaderName + to_string(m_iCurSerealNum));
		pTreeObject->SetId(m_iCurSerealNum++);
		objectList.emplace_back(pTreeObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}