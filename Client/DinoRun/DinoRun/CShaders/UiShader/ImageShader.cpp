#include "UiShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "../../Meshes/PlaneMesh.h"

ImageShader::ImageShader()
{
}
ImageShader::~ImageShader()
{
}


void ImageShader::BuildObjects(void* pInformation)
{	
	UI_INFO* ui_Infomation = (UI_INFO*)pInformation;
	if (ui_Infomation->positions.size() <= 0)
		return;
	CTexture * texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	texture->LoadTextureFromFile(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get(), ConvertCHARtoWCHAR(ui_Infomation->textureName.c_str()), 0);

	CreateCbvSrvDescriptorHeaps(0, 1);

	CreateShaderResourceViews(texture, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(texture);
	material->CreateShaderVariable(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(ui_Infomation->meshSize.x, ui_Infomation->meshSize.y, 0.1f, ui_Infomation->minUv.x, 
		ui_Infomation->maxUv.x, ui_Infomation->minUv.y, ui_Infomation->maxUv.y);
	mesh->CreateShaderVariables(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());

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

	CreateShaderVariables();

}


void ImageShader::Update(float fTimeElapsed, void* pInformation)
{

}