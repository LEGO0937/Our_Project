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
	string.insert(string.find("."), "_ins");  //�ν��Ͻ� ���� �������� �ҷ��´�
											  //�ؽ�ó,�޽��� �������� ��� ��������.
	const char* fileName = string.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pTreeObject = pModel->m_pModelRootObject;
		pTreeObject->AddRef();
		
		//�̰����� findFrame�� ���� �� ������Ʈ�� ���� �� Ű�׸�ƽ �� �߰��� ��.
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