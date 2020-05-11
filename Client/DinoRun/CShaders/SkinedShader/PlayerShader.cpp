#include "SkinedShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Common/Animation/Animation.h"

PlayerShader::PlayerShader()
{
}
PlayerShader::~PlayerShader()
{
}

void PlayerShader::BuildObjects(CreateManager* pCreateManager, const char *pszFileName, const char* filename)
{
	
	if (!pszFileName)
		return;
	instancingModelName = pszFileName;
	instancingModelName.insert(instancingModelName.find("."), "_ins");  //�ν��Ͻ� ���� �������� �ҷ��´�

	CDinoRunPlayer *pModel = new CDinoRunPlayer(pCreateManager, pszFileName);
	m_ppSkinedObjects = pModel;
	//m_ppSkinedObjects->AddRef();

	if (pszFileName)
		Load(pCreateManager, pszFileName, filename);

	CreateShaderVariables(pCreateManager);
	
}


void PlayerShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	CPlayer *pPlayerObject = NULL;
	int nLength = 1;

	//nLenght�� �÷��̾� ���� ����.
	for (int i = 0; i < nLength; ++i)
	{
		pPlayerObject = new CDinoRunPlayer(pCreateManager, instancingModelName);
		//pPlayerObject->AddRef();
		//pPlayerObject->SetPosition(XMFLOAT3(800.0f, 73.6, 920));
		//pPlayerObject->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		//pPlayerObject->OnPrepareRender();
		m_vSkinedObjectList.emplace_back(pPlayerObject);
	}
}

