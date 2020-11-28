#include "UiShader.h"
#include "../Common/FrameWork/GameManager.h"

#include "../../Objects/PlayerObject.h"
#include "../../Meshes/PlaneMesh.h"

GaugeShader::GaugeShader()
{
}
GaugeShader::~GaugeShader()
{
}


void GaugeShader::BuildObjects(void* pInformation)
{
	
	CTexture * Guage = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	Guage->LoadTextureFromFile(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get(), L"Resources/Images/T_Gauge.dds", 0);

	CreateCbvSrvDescriptorHeaps(0, 1);

	CreateShaderResourceViews(Guage, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(Guage);
	material->CreateShaderVariable(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(0.003f,0.06f,0.1f,0.0f,1.0f,0.0f,1.0f);
	mesh->CreateShaderVariables(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);
	
	pObject = new CGameObject;
	pObject->SetPosition(-0.27f, -0.85f, 0.0f);
	pObject->AddRef();

	objectList.emplace_back(pObject);

	uvX.emplace_back(0);
	uvY.emplace_back(0);

	CreateShaderVariables();
	
}


void GaugeShader::Update(float fTimeElapsed, void* pInformation)
{
	CPlayer* pPlayer = (CPlayer*)pInformation;
	UINT nGauge = (UINT)pPlayer->GetMaxVelocityXZ();
	pPlayer->SetMaxVelocityXZ(pPlayer->GetMaxVelocityXZ() - fTimeElapsed * 0.5f);
	if (pPlayer->GetMaxVelocityXZ() < MIN_VELOCITY)
		pPlayer->SetMaxVelocityXZ(MIN_VELOCITY);
	if (uvX[0] < (int)(nGauge * 2))
	{
		uvX[0] += 1;
	}
	else if (uvX[0] > (int)(nGauge * 2))
	{
		uvX[0] -= 1;
	}
}