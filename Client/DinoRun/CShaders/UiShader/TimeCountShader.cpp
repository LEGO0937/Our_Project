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


void TimeCountShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain)
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
	mesh = new PlaneMesh(0.03, 0.045, 0.1,0,0.1,0,0.5);
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);
	
	vNum.reserve(8);

	pObject = new CGameObject;
	pObject->SetPosition(0.62, 0.75, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	vNum.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.67, 0.75, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	vNum.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.77, 0.75, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	vNum.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.82, 0.75, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	vNum.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.92, 0.75, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	vNum.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.97, 0.75, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	vNum.emplace_back(0);

	pObject = new CGameObject;
	pObject->SetPosition(0.72, 0.75, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	vNum.emplace_back(10);
	
	pObject = new CGameObject;    //6자리
	pObject->SetPosition(0.87, 0.75, 0);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	vNum.emplace_back(10);
	// m_ppObjects의 m_pcbMappedGameObjects를 통해 숫자배열이미지의 인덱스값 전송 가능

	CreateShaderVariables(pCreateManager);
}

void TimeCountShader::Update(float fTimeElapsed, CPlayer* player)
{
	m_fSecond += fTimeElapsed;
	if (m_fSecond >= 60)
	{
		m_iMinute++;
		m_fSecond -= 60;
	}

	vNum[0] = m_iMinute / 10;
	vNum[1] = m_iMinute % 10;

	vNum[2] =  m_fSecond / 10;
	vNum[3] = (int)m_fSecond % 10;

	int n = (int)(m_fSecond * 100) % 100;
	vNum[4] = n / 10;
	vNum[5] = n % 10;
}