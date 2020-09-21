#include "UiShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "../Common/FrameWork/SoundManager.h"
#include "../../Meshes/PlaneMesh.h"

CountDownShader::CountDownShader()
{
}
CountDownShader::~CountDownShader()
{
}


void CountDownShader::BuildObjects(void* pInformation)
{

	CTexture * Count = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	Count->LoadTextureFromFile(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get(), L"Resources/Images/T_StartText.dds", 0);

	CreateCbvSrvDescriptorHeaps(0, 1);

	CreateShaderResourceViews(Count, 8, true);

	m_ppObjects = new CGameObject(1);
	m_ppObjects->AddRef();

	CGameObject *pObject = NULL;
	CMaterial *material = new CMaterial(1);

	material->SetTexture(Count);
	material->CreateShaderVariable(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());
	m_ppObjects->SetMaterial(0, material);

	PlaneMesh *mesh = NULL;
	mesh = new PlaneMesh(0.05f, 0.065f, 0.1f, 0.0f, 0.25f, 0.0f, 1.0f);
	mesh->CreateShaderVariables(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);

	uvX.reserve(1);
	uvY.reserve(1);

	pObject = new CGameObject;    //6ÀÚ¸®
	pObject->SetPosition(0.0f, 0.5f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0.76f);
	uvY.emplace_back(0.0f);

	m_fScaleValue = 4.0f;
	objectList[0]->m_xmf4x4World._11 = m_fScaleValue;
	objectList[0]->m_xmf4x4World._22 = m_fScaleValue;

	CreateShaderVariables();
}

void CountDownShader::Update(float fTimeElapsed, void* pInformation)
{
	//vNum = 181
	if (!isEnable)
		return;
	float* time = (float*)pInformation;
	int t = (int)*time;
	if (uvX[0] != t*0.25f)
	{
		uvX[0] = 0.25f * t;
		m_fScaleValue = 4.0f;
		objectList[0]->m_xmf4x4World._11 = m_fScaleValue;
		objectList[0]->m_xmf4x4World._22 = m_fScaleValue;
		
		if (t != 0.0f)
			SoundManager::GetInstance()->Play("CountDown",0.2f);
		else
			SoundManager::GetInstance()->Play("CountDownZero",0.2f);
	}

	if (m_fScaleValue > 2.0f)
	{
		m_fScaleValue -= 0.5f;
		objectList[0]->m_xmf4x4World._11 = m_fScaleValue;
		objectList[0]->m_xmf4x4World._22 = m_fScaleValue;
	}
	
	if (uvX[0] == 0.0f)
	{
		m_fFinishTime += fTimeElapsed;
		if (m_fFinishTime > 1.5f)
			isEnable = false;
	}
}