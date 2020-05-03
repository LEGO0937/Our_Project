#include "UiShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Meshes/PlaneMesh.h"

TrackCountShader::TrackCountShader()
{
}
TrackCountShader::~TrackCountShader()
{
}


void TrackCountShader::BuildObjects(CreateManager* pCreateManager, void* pInformation)
{

	CTexture * Count = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	Count->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), L"Resources/Images/Time_Number.dds", 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, Count, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(Count);
	material->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(0.03f, 0.045f, 0.1f, 0.0f, 0.1f, 0.0f, 0.5f);
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);

	uvX.reserve(3);
	uvY.reserve(3);

	pObject = new CGameObject;    //6ÀÚ¸®
	pObject->SetPosition(0.87f, 0.84f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.92f, 0.84f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(11);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.97f, 0.84f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(2);
	uvY.emplace_back(0);

	CreateShaderVariables(pCreateManager);
}

void TrackCountShader::Update(float fTimeElapsed, void* pInformation)
{
	//vNum = 181
	CPlayer* pPlayer = (CPlayer*)pInformation;
	uvX[0] = pPlayer->GetCheckPoint() / 181;
}