#include "UiShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/PlayerObject.h"
#include "../../Meshes/PlaneMesh.h"

GaugeShader::GaugeShader()
{
}
GaugeShader::~GaugeShader()
{
}


void GaugeShader::BuildObjects(CreateManager* pCreateManager, void* pInformation)
{
	
	CTexture * Guage = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	Guage->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), L"Resources/Images/gauge.dds", 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, Guage, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(Guage);
	material->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(0.003f,0.06f,0.1f,0.0f,1.0f,0.0f,1.0f);
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);
	
	pObject = new CGameObject;
	pObject->SetPosition(-0.27, -0.85, 0);
	pObject->AddRef();

	objectList.emplace_back(pObject);

	uvX.emplace_back(0);
	uvY.emplace_back(0);

	CreateShaderVariables(pCreateManager);
	
}


void GaugeShader::Update(float fTimeElapsed, void* pInformation)
{
	CPlayer* pPlayer = (CPlayer*)pInformation;
	UINT nGauge = (UINT)pPlayer->GetMaxForce();
	pPlayer->SetMaxForce(pPlayer->GetMaxForce() - fTimeElapsed * 2);
	if (pPlayer->GetMaxForce() < MIN_FORCE) pPlayer->SetMaxForce(MIN_FORCE);
	if (uvX[0] < (int)(nGauge * 0.05))
	{
		uvX[0] += 1;
	}
	else if (uvX[0] > (int)(nGauge * 0.05))
	{
		uvX[0] -= 1;
	}
}