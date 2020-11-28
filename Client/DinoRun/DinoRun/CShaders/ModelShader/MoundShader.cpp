#include "ModelShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "ItemObject.h"

MoundShader::MoundShader()
{
	shaderName = _MOUND_SHADER;
}
MoundShader::~MoundShader()
{

}

void MoundShader::Load(const char* filename, const char* Loadname)
{
	billBoardCb->fSize = 50;
	isEnable = true;

	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	ItemMound* pItemObject = NULL;
	UINT nReads;
	int nLength = 0;

	const char* fileName = instancingModelName.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(fileName, NULL);
		pItemObject = new ItemMound;
		pItemObject->SetChild(pModel->m_pModelRootObject->GetChild());
		pItemObject->AddRef();

		//�̰����� findFrame�� ���� �� ������Ʈ�� ���� �� Ű�׸�ƽ �� �߰��� ��.
		nReads = (UINT)::fread(&(pItemObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		pItemObject->SetName(shaderName + to_string(m_iCurSerealNum));
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

void MoundShader::Update(float fTimeElapsed)
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

void MoundShader::addObject(const XMFLOAT4X4& xmf3DepartPosition)
{
	ItemMound* pItemObject = NULL;
	CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(instancingModelName.c_str(), NULL);

	pItemObject = new ItemMound;
	pItemObject->SetChild(pModel->m_pModelRootObject->GetChild());
	pItemObject->AddRef();

	pItemObject->SetMatrix(xmf3DepartPosition);
	pItemObject->SetName(shaderName + to_string(m_iCurSerealNum));
	pItemObject->SetId(m_iCurSerealNum++);
	pItemObject->UpdateTransform(NULL);

	objectList.emplace_back(pItemObject);
	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}
	ReleaseShaderVariables();
	CreateShaderVariables();
}