#include "UiShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Meshes/PlaneMesh.h"

IconShader::IconShader()
{
}
IconShader::~IconShader()
{
}


void IconShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, void* pInformation)
{

	string* name = (string*)pInformation;
	CTexture * Icon = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	Icon->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(name->c_str()), 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, Icon, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(Icon);
	material->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(0.1, 0.1, 0.5, 0, 1.0, 0, 1.0);
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


void IconShader::Update(float fTimeElapsed, void* pInformation)
{

}