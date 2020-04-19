#include "UiShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Meshes/PlaneMesh.h"

BackGroundShader::BackGroundShader()
{
}
BackGroundShader::~BackGroundShader()
{
}


void BackGroundShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain)
{

	string* name = (string*)terrain;
	CTexture * backGround = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	backGround->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(name->c_str()), 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, backGround, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(backGround);
	material->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(1, 1, 0.1, 0, 1.0, 0, 1.0);
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);

	pObject = new CGameObject;
	pObject->SetPosition(0, 0, 0);
	pObject->AddRef();

	objectList.emplace_back(pObject);

	uvX.emplace_back(0);
	uvY.emplace_back(0);

	CreateShaderVariables(pCreateManager);

}


void BackGroundShader::Update(float fTimeElapsed, CPlayer* player)
{
	
}