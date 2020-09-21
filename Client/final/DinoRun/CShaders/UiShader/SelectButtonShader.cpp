#include "UiShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "../../Meshes/PlaneMesh.h"

SelectImageShader::SelectImageShader()
{
}
SelectImageShader::~SelectImageShader()
{
}


void SelectImageShader::BuildObjects(void* pInformation)
{

	const char* name = *(const char**)pInformation;
	CTexture * backGround = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	backGround->LoadTextureFromFile(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get(), ConvertCHARtoWCHAR(name), 0);

	CreateCbvSrvDescriptorHeaps(0, 1);

	CreateShaderResourceViews(backGround, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(backGround);
	material->CreateShaderVariable(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(1.0f, 1.0f, 0.1f, 0.0f, 1.0f, 0.0f, 1.0f);
	mesh->CreateShaderVariables(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);

	pObject = new CGameObject;
	pObject->SetPosition(0.0f, 0.0f, 0.0f);
	pObject->AddRef();

	objectList.emplace_back(pObject);

	uvX.emplace_back(0.0f);
	uvY.emplace_back(0.0f);

	CreateShaderVariables();
}


void SelectImageShader::Update(float fTimeElapsed, void* pInformation)
{

}