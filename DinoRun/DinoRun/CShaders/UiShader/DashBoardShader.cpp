#include "UiShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Meshes/PlaneMesh.h"

DashBoardShader::DashBoardShader()
{
	shaderName = "DashBoardShader";
}
DashBoardShader::~DashBoardShader()
{
}


void DashBoardShader::BuildObjects(CreateManager* pCreateManager, void* pInformation)
{

	CTexture * Count = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	Count->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), L"Resources/Images/T_Itembox.dds", 0);

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
	mesh = new PlaneMesh(0.12f, 0.12f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	mesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_ppObjects->SetMesh(mesh);

	uvX.reserve(1);
	uvY.reserve(1);

	pObject = new CGameObject;    //6자리
	pObject->SetPosition(0.72f, -0.75f, 0.0f);
	pObject->AddRef();
	objectList.emplace_back(pObject);
	uvX.emplace_back(0);
	uvY.emplace_back(0);

	CreateShaderVariables(pCreateManager);
}

void DashBoardShader::Update(float fTimeElapsed, void* pInformation)
{
	//vNum = 181
	CPlayer* pPlayer = (CPlayer*)pInformation;
	XMFLOAT3 vel = pPlayer->GetVelocity();

	int lengthVelocity = sqrt(vel.x * vel.x + vel.z * vel.z);
	//현재 속도는 m/s 단위이므로 km/h로 바꿔줘야함.
	lengthVelocity = lengthVelocity * 3600 * KILOMETER_PER_METER;

	//m_ppObjects[0].m_xmf4x4World;
	//SetPosition(0.72f, -0.75f, 0.0f);
	XMMATRIX mtxRotate = XMMatrixRotationZ(XMConvertToRadians(-lengthVelocity - 30));

	XMStoreFloat4x4(&objectList[0]->m_xmf4x4ToParent, mtxRotate);
	objectList[0]->SetPosition(0.72f, -0.75f, 0.0f);

	//uvX[0] = (int)(lengthVelocity * 0.01) % 10;
}