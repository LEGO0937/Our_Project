#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/ItemObject.h"

StoneShader::StoneShader()
{
}
StoneShader::~StoneShader()
{

}

void StoneShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	billBoardCb->fSize = 50;
	isEnable = true;

	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	ItemStone* pItemObject = NULL;
	UINT nReads;
	int nLength = 0;

	const char* fileName = instancingModelName.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pItemObject = new ItemStone;
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

void StoneShader::Update(float fTimeElapsed)
{
	// �����κ��� �߰� ��ȣ�� �ް� addObject(m_pCreateManager.get(), matrix)ȣ��
	// ���� ��ɵ� ���⼭ �ް� ������ ��. ���� ����� ���̵�� ã�� ��.
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

void StoneShader::addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition, const XMFLOAT4X4& xmf3ArrivePosition)
{
	ItemStone* pItemObject = NULL;
	CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, instancingModelName.c_str(), NULL);

	pItemObject = new ItemStone;
	pItemObject->SetChild(pModel->m_pModelRootObject->GetChild());
	pItemObject->AddRef();

	pItemObject->SetMatrix(xmf3DepartPosition);

	objectList.emplace_back(pItemObject);
	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}
	ReleaseShaderVariables();
	CreateShaderVariables(pCreateManager);
}