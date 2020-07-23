#include "SkinedShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Common/Animation/Animation.h"

PlayerShader::PlayerShader()
{
}
PlayerShader::~PlayerShader()
{
}
void PlayerShader::BuildObjects(CreateManager* pCreateManager, void* pInformation)
{
	MODEL_INFO* info = (MODEL_INFO*)pInformation;
	if (!info->modelName)
		return;
	if (info->updatedContext)
		m_pUpdatedContext = info->updatedContext;

	instancingModelName = info->modelName;
	instancingModelName.insert(instancingModelName.find("."), "_ins");  //�ν��Ͻ� ���� �������� �ҷ��´�

	CDinoRunPlayer *pModel = new CDinoRunPlayer(pCreateManager, info->modelName);
	m_ppSkinedObjects = pModel;
	//m_ppSkinedObjects->AddRef();

	if (info->modelName)
		Load(pCreateManager, info->modelName, info->dataFileName);

	CreateShaderVariables(pCreateManager);
}

void PlayerShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	CPlayer *pPlayerObject = NULL;
	int nLength = 3;

	//�����κ��� nLenght�� �÷��̾� ���� ����.
	
	for (int i = 0; i < nLength; ++i)
	{
		pPlayerObject = new CDinoRunPlayer(pCreateManager, instancingModelName);
		//���ÿ� ���̵� �޾Ƽ� ����� ����.
		//pPlayerObject->AddRef();
		//pPlayerObject->SetPosition(XMFLOAT3(800.0f, 73.6, 920));
		//pPlayerObject->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		//pPlayerObject->OnPrepareRender();
		//pPlayerObject->SetId(m_iCurSerealNum++);
		pPlayerObject->UpCheckPoint();
		pPlayerObject->UpCheckPoint();
		pPlayerObject->UpCheckPoint();
		//�ִϸ��̼� ������ find_if�� ���ؼ� ���̵�� �´� ������Ʈ ã�� �����ϵ��� ����.
		m_vSkinedObjectList.emplace_back(pPlayerObject);
	}
}

void PlayerShader::Update(float fTimeElapsed)
{
	//�� �������� �����κ��� �޽��� �ް� Ư�� ������ �ִϸ��̼� ���� ���� �� 
	//����Ʈ���� ���̵�� ��ġ�ϴ� ������Ʈ ã�Ƽ� �ִϸ��̼� ������.
}
