#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "ItemObject.h"

ItemShader::ItemShader()
{
}
ItemShader::~ItemShader()
{

}

void ItemShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	isEnable = true;

	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	 ItemObject*pItemObject = NULL;
	UINT nReads;
	int nLength = 0;

	const char* fileName = instancingModelName.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pItemObject = new ItemObject;
		pItemObject->SetChild(pModel->m_pModelRootObject->GetChild());
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

void ItemShader::Update(float fTimeElapsed)
{
	// 서버로부터 비활성화, 활성화 해야하는 오브젝트가 있는지 확인,있을 경우 enable 셋팅해줄 것.
	// 하지만 아이템박스와 마찬가지로 활성화 시키기 위해 시간을 재는건 서버가 해야할 듯?
	// 클라에서 하면 모든 클라에서 활성화 신호를 보내서 중복된 메시지를 전송할 것임.

	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Update(fTimeElapsed, NULL);
		}
	}
}