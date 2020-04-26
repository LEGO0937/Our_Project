#include "UiShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Objects/TerrainObject.h"
#include "../../Meshes/PlaneMesh.h"

TimeCountShader::TimeCountShader()
{
}
TimeCountShader::~TimeCountShader()
{
}


void TimeCountShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, void* pInformation)
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
	mesh = new PlaneMesh(0.03f, 0.045f, 0.1f,0.0f,0.1f,0.0f,0.5f);
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);
	
	uvX.reserve(8);
	uvY.reserve(8);

	pObject = new CGameObject;
	pObject->SetPosition(0.62f, 0.75f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.67f, 0.75f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.77f, 0.75f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.82f, 0.75f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.92f, 0.75f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.97f, 0.75f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0);
	uvY.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.72f, 0.75f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(10);
	uvY.emplace_back(0);

	pObject = new CGameObject;    //6자리
	pObject->SetPosition(0.87f, 0.75f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(10);
	uvY.emplace_back(0);
	// m_ppObjects의 m_pcbMappedGameObjects를 통해 숫자배열이미지의 인덱스값 전송 가능

	CreateShaderVariables(pCreateManager);
}

void TimeCountShader::Update(float fTimeElapsed, void* pInformation)
{
	m_fSecond += fTimeElapsed;
	if (m_fSecond >= 60)
	{
		m_iMinute++;
		m_fSecond -= 60;
	}

	uvX[0] = m_iMinute / 10;
	uvX[1] = m_iMinute % 10;

	uvX[2] =  m_fSecond / 10;
	uvX[3] = (int)m_fSecond % 10;

	int n = (int)(m_fSecond * 100) % 100;
	uvX[4] = n / 10;
	uvX[5] = n % 10;
}