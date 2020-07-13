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

		//�̰����� findFrame�� ���� �� ������Ʈ�� ���� �� Ű�׸�ƽ �� �߰��� ��.
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
	// �����κ��� ��Ȱ��ȭ, Ȱ��ȭ �ؾ��ϴ� ������Ʈ�� �ִ��� Ȯ��,���� ��� enable �������� ��.
	// ������ �����۹ڽ��� ���������� Ȱ��ȭ ��Ű�� ���� �ð��� ��°� ������ �ؾ��� ��?
	// Ŭ�󿡼� �ϸ� ��� Ŭ�󿡼� Ȱ��ȭ ��ȣ�� ������ �ߺ��� �޽����� ������ ����.

	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Update(fTimeElapsed, NULL);
		}
	}
}