#include "UiShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/TerrainObject.h"
#include "../../Meshes/PlaneMesh.h"

TrackCountShader::TrackCountShader()
{
}
TrackCountShader::~TrackCountShader()
{
}


void TrackCountShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain)
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
	mesh = new PlaneMesh(0.03, 0.045, 0.1, 0, 0.1, 0, 0.5);
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);

	uvX.reserve(3);
	uvY.reserve(3);

	pObject = new CGameObject;    //6자리
	pObject->SetPosition(0.87, 0.84, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.92, 0.84, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(11);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.97, 0.84, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(2);
	uvY.emplace_back(0);
	// m_ppObjects의 m_pcbMappedGameObjects를 통해 숫자배열이미지의 인덱스값 전송 가능

	CreateShaderVariables(pCreateManager);
}

void TrackCountShader::Update(float fTimeElapsed, CPlayer* player)
{
	//vNum = 181
	uvX[0] = player->GetCheckPoint() / 181;
}