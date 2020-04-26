#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"

TreeShader::TreeShader()
{
}
TreeShader::~TreeShader()
{
}

void TreeShader::Load(shared_ptr<CreateManager> pCreateManager, const char* filename, const char* Loadname)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	CGameObject *pTreeObject = NULL;
	UINT nReads;
	int nLength = 0;

	string string = filename;
	string.insert(string.find("."), "_ins");  //인스턴싱 전용 모델파일을 불러온다
											  //텍스처,메시의 정보들이 들어 있지않음.
	const char* fileName = string.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pTreeObject = pModel->m_pModelRootObject;
		pTreeObject->AddRef();
		
		//이곳에서 findFrame을 통해 각 오브젝트에 질량 및 키네마틱 값 추가할 것.
		nReads = (UINT)::fread(&(pTreeObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		pTreeObject->UpdateTransform(NULL);
		objectList.emplace_back(pTreeObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}