#include "UiShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "../Common/FrameWork/NetworkManager.h"
#include "../../Meshes/PlaneMesh.h"

RankCountShader::RankCountShader()
{
}
RankCountShader::~RankCountShader()
{
}


void RankCountShader::BuildObjects(void* pInformation)
{

	CTexture * Count = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	Count->LoadTextureFromFile(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get(), L"Resources/Images/Time_Number.dds", 0);

	CreateCbvSrvDescriptorHeaps(0, 1);

	CreateShaderResourceViews(Count, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(Count);
	material->CreateShaderVariable(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(0.05f, 0.065f, 0.1f, 0.0f, 0.1f, 0.0f, 0.5f);
	mesh->CreateShaderVariables(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);

	uvX.reserve(3);
	uvY.reserve(3);

	pObject = new CGameObject;    //6ÀÚ¸®
	pObject->SetPosition(-0.95f, 0.74f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(1);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(-0.88f, 0.74f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(11);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(-0.81f, 0.74f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(5);
	uvY.emplace_back(0);

	for (int i = 0; i <= NetWorkManager::GetInstance()->GetNumPlayer(); ++i)
	{
		pObject = new CGameObject;
		pObject->SetPosition(-0.96f, 0.54f - 0.12*i, 0.0f);
		pObject->AddRef();
		objectList.emplace_back(pObject);
		uvX.emplace_back(i+1);
		uvY.emplace_back(0);
	}
	//pObject = new CGameObject;
	//pObject->SetPosition(-0.96f, 0.42f, 0.0f);
	//pObject->AddRef();
	//objectList.emplace_back(pObject);
	//uvX.emplace_back(8);
	//uvY.emplace_back(0);

	CreateShaderVariables();
}

void RankCountShader::Update(float fTimeElapsed, void* pInformation)
{
	//vNum = 181
	CPlayer* pPlayer = (CPlayer*)pInformation;
	uvX[0] = pPlayer->GetRank();
}