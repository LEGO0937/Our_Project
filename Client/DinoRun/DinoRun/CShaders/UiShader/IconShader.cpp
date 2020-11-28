#include "UiShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "../../Meshes/PlaneMesh.h"

IconShader::IconShader()
{
}
IconShader::~IconShader()
{
}


void IconShader::BuildObjects(void* pInformation)
{

	string* name = (string*)pInformation;
	CTexture * Icon = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	Icon->LoadTextureFromFile(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get(), ConvertCHARtoWCHAR(name->c_str()), 0);

	CreateCbvSrvDescriptorHeaps(0, 1);

	CreateShaderResourceViews(Icon, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(Icon);
	material->CreateShaderVariable(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(0.1f, 0.1f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f);
	mesh->CreateShaderVariables(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);

	pObject = new CGameObject;
	pObject->SetPosition(0.0f, 0.0f, 0.0f);
	pObject->AddRef();

	objectList.emplace_back(pObject);

	uvX.emplace_back(0);
	uvY.emplace_back(0);

	CreateShaderVariables();

}


void IconShader::Update(float fTimeElapsed, void* pInformation)
{

}