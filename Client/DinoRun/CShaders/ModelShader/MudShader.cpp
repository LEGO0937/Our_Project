#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "ItemObject.h"

MudShader::MudShader()
{
	shaderName = "MudShader";
}
MudShader::~MudShader()
{

}

void MudShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	isEnable = true;

	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	ItemMud* pItemObject = NULL;
	UINT nReads;
	int nLength = 0;

	const char* fileName = instancingModelName.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pItemObject = new ItemMud;
		pItemObject->SetChild(pModel->m_pModelRootObject->GetChild());
		pItemObject->AddRef();

		//�̰����� findFrame�� ���� �� ������Ʈ�� ���� �� Ű�׸�ƽ �� �߰��� ��.
		nReads = (UINT)::fread(&(pItemObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		pItemObject->SetId(m_iCurSerealNum++);

		objectList.emplace_back(pItemObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}

void MudShader::Update(float fTimeElapsed)
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

void MudShader::addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition, const XMFLOAT4X4& xmf3ArrivePosition)
{
	ItemMud* pItemObject = NULL;
	CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, instancingModelName.c_str(), NULL);

	pItemObject = new ItemMud;
	pItemObject->SetChild(pModel->m_pModelRootObject->GetChild());
	pItemObject->AddRef();

	pItemObject->SetMatrix(xmf3DepartPosition);
	pItemObject->SetId(m_iCurSerealNum++);

	objectList.emplace_back(pItemObject);
	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}
	ReleaseShaderVariables();
	CreateShaderVariables(pCreateManager);
}