#include "UiShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Meshes/PlaneMesh.h"

ButtonShader::ButtonShader()
{
}
ButtonShader::~ButtonShader()
{
}


void ButtonShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, void* pInformation)
{	
	UI_INFO* ui_Infomation = (UI_INFO*)pInformation;
	if (ui_Infomation->positions.size() <= 0)
		return;
	CTexture * texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	texture->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(ui_Infomation->textureName.c_str()), 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, texture, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(texture);
	material->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(ui_Infomation->meshSize.x, ui_Infomation->meshSize.y, 0.1, 0, 0.5, 0, 0.25);
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);

	uvX.reserve(ui_Infomation->positions.size());
	uvY.reserve(ui_Infomation->positions.size());

	for (int i = 0; i < ui_Infomation->positions.size(); ++i)
	{
		pObject = new CGameObject;
		pObject->SetPosition(ui_Infomation->positions[i]);

		uvX.emplace_back(0);
		uvY.emplace_back(ui_Infomation->f_uvY[i]);

		pObject->AddRef();
		objectList.emplace_back(pObject);
	}

	CreateShaderVariables(pCreateManager);

}


void ButtonShader::Update(float fTimeElapsed, void* pInformation)
{

}