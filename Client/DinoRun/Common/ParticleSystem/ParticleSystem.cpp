#include "ParticleSystem.h"
#include <time.h>


ParticleSystem::ParticleSystem(shared_ptr<CreateManager> pCreateManager, const char& cPattern, 
	const char& cShape, const float& fGravity,	const UINT& uSize,	CGameObject* pTarget, 
	const XMFLOAT3& xmf3Position, const float& fVelocity,	string pTextureName, 
	const float& fLife, const UINT& uMaxSize): m_cPattern(cPattern),m_cShape(cShape), m_uSize(uSize),
	m_fParticleLife(fLife), m_fVelocity(fVelocity),m_fGravity(fGravity),m_uMaxSize(uMaxSize)
{
	m_pd3dCommandList = pCreateManager->GetCommandList();

	CTexture * texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	texture->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(pTextureName.c_str()), 0);

    m_pShader = new CShader();
	m_pShader->CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);
	m_pShader->CreateShaderResourceViews(pCreateManager, texture, 8, true);

	m_pMaterial = new CMaterial(1);
	m_pMaterial->SetShader(m_pShader);
	m_pMaterial->SetTexture(texture, 0);

	m_pMaterial->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_pMesh = new BillBoardMesh();
	m_pMesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
	
	
	m_vParticles.reserve(m_uMaxSize);

	m_pTarget = pTarget;

	m_xmf3Position = xmf3Position;

	BuildResource(pCreateManager);
	//CreateParticles();
}
ParticleSystem::~ParticleSystem()
{
	ReleaseUploadBuffers();

	if (m_pMesh)
	{
		m_pMesh->ReleaseShaderVariables();
		m_pMesh->Release();
		m_pMesh = NULL;
	}
	if (m_pMaterial)
	{
		m_pMaterial->Release();
	}
	if (m_pd3dUbParticles)
	{
		m_pd3dUbParticles->Release();
		m_pd3dUbParticles = NULL;
	}

	if (m_pd3dReadBackParticles)
	{
		if (m_pReadBackMappedParticles)
			m_pd3dReadBackParticles->Unmap(0, NULL);
		m_pd3dReadBackParticles->Release();
		m_pd3dReadBackParticles = NULL;
	}
	if (m_pd3dSrbParticles)
	{
		m_pd3dSrbParticles->Unmap(0, NULL);
		m_pd3dSrbParticles->Release();
		m_pd3dSrbParticles = NULL;
	}
	if (m_pd3dcbStruct)
	{
		m_pd3dcbStruct->Unmap(0, NULL);
		m_pd3dcbStruct->Release();
		m_pd3dcbStruct = NULL;
	}
	//m_vParticles.clear();
}

bool ParticleSystem::AnimateObjects(float fTimeElapsed) 
{
	D3D12_RANGE readbackBufferRange{ 0,sizeof(Particle) * m_vParticles.size() };
	m_pd3dReadBackParticles->Map(0, &readbackBufferRange, (void **)&m_pReadBackMappedParticles);
	memcpy(m_pSrbMappedParticles, m_pReadBackMappedParticles, sizeof(Particle) * m_vParticles.size());
	memcpy(m_vParticles.data(), m_pSrbMappedParticles, sizeof(Particle) * m_vParticles.size());
	m_pd3dReadBackParticles->Unmap(0, NULL);
	m_pReadBackMappedParticles = NULL;

	particleCb->fElapsedTime = fTimeElapsed;
	particleCb->fGravity = m_fGravity;
	if (m_bEnable)
	{
		if (curNumParticle + m_cShape > m_uMaxSize)
		{
			if (m_cPattern == ONES)
			{

				if (m_vParticles.size() == 0)
				{
					return true; //메모리 반납   씬에서 release 처리
				}
			}
			else if (m_cPattern == LOOP)
			{
				curNumParticle = 0;  //루프는 소유자가 직접 release해야만 함
			}
		}
		else
		{
			Update(fTimeElapsed);
		}
	}
	for (auto i = m_vParticles.begin(); i < m_vParticles.end(); )
	{
		if (i->life < 0)
		{
			i = m_vParticles.erase(i);
		}
		else
			++i;
	}
	memcpy(m_pSrbMappedParticles, m_vParticles.data(), sizeof(Particle) * m_uMaxSize);
	ChangeResourceState(m_pd3dCommandList.Get(), m_pd3dUbParticles, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	m_pd3dCommandList->SetComputeRootShaderResourceView(4,
		m_pd3dSrbParticles->GetGPUVirtualAddress());
	m_pd3dCommandList->SetComputeRootUnorderedAccessView(5,
		m_pd3dUbParticles->GetGPUVirtualAddress());
	m_pd3dCommandList->SetComputeRootConstantBufferView(6, m_pd3dcbStruct->GetGPUVirtualAddress());

	m_pd3dCommandList->Dispatch(1, 1, 1);

	ChangeResourceState(m_pd3dCommandList.Get(), m_pd3dUbParticles, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	m_pd3dCommandList->CopyResource(m_pd3dReadBackParticles, m_pd3dUbParticles);
	return false;
}
void ParticleSystem::CreateParticles()
{

	XMFLOAT3 pos;
	if (m_pTarget)
	{
		//타겟의 월드행렬에 오프셋값(position)의 translate 행렬곱한 결과의 포지션값을 구해온다.
		//XMFLOAT4X4 matrix = Matrix4x4::Multiply(m_pTarget->m_xmf4x4World,
		//	XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z));
		//pos.x = matrix._41; pos.y = matrix._42; pos.z = matrix._43;
		//위치값 손봐야함;
		pos = m_pTarget->GetPosition();
	}
	else
		pos = m_xmf3Position;

	//XMFLOAT3 vel = XMFLOAT3(m_xmf3Velocity.x* cos(rand()), m_xmf3Velocity.y,
	//	m_xmf3Velocity.z*(cos(rand()) + sin(rand()))*0.5);

	//m_vParticles.emplace_back(Particle(m_xmf3Position, vel, m_fParticleLife));

	switch (m_cShape)
	{
	case CONE:
		if (curNumParticle + 15 < m_uMaxSize)
		{
			XMFLOAT3 vel = XMFLOAT3(0.5f*m_fVelocity, 1.0f * m_fVelocity, 0.0f);
			if (m_pTarget)
			{
				vel = Vector3::TransformCoord(vel, m_pTarget->m_xmf4x4World);
				vel = Vector3::Normalize(vel);
				vel = XMFLOAT3(vel.x*0.5f, vel.x, 0.0f);
			}


			XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			XMVECTOR up = XMLoadFloat3(&xmf3Up);

			XMMATRIX mat = XMMatrixRotationAxis(up, XMConvertToRadians(24));
			for (int i = 0; i < 15; ++i)
			{
				m_vParticles.emplace_back(Particle(pos, vel, m_fParticleLife));
				vel = Vector3::TransformNormal(vel, mat);
			}
			curNumParticle += 15;
		}
		break;
	case RAND:
		if (curNumParticle < m_uMaxSize)
		{
			XMFLOAT3 vel = XMFLOAT3(m_fVelocity*cos(rand()), m_fVelocity,
				m_fVelocity * (cos(rand()) + sin(rand()))*0.5f);

			m_vParticles.emplace_back(Particle(pos, vel, m_fParticleLife));
			curNumParticle++;
		}
		break;
	case BOOM:
		break;
	default:
		break;
	}
}
void ParticleSystem::Update(float fTimeElapsed) 
{

	srand(time(NULL));
	coolTime += fTimeElapsed;
	
	if (coolTime > 0.25)
	{
		coolTime = 0.f;
		if (m_vParticles.size() < m_uMaxSize)
		{
			//if (m_pTarget)
			//{
			//	//타겟의 월드행렬에 오프셋값(position)의 translate 행렬곱한 결과의 포지션값을 구해온다.
			//	XMFLOAT4X4 matrix = Matrix4x4::Multiply(m_pTarget->m_xmf4x4World,
			//		XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z));
			//	m_xmf3Position.x = matrix._41; m_xmf3Position.y = matrix._42; m_xmf3Position.z = matrix._43;
			//
			//	m_xmf3Position.x = (((float)rand() - (float)rand()) / RAND_MAX) * m_xmf3Position.x;
			//	m_xmf3Position.y = (((float)rand() - (float)rand()) / RAND_MAX) * m_xmf3Position.y;
			//	m_xmf3Position.z = (((float)rand() - (float)rand()) / RAND_MAX) * m_xmf3Position.z;
			//}

			//XMFLOAT3 vel = XMFLOAT3(m_xmf3Velocity.x* cos(rand()), m_xmf3Velocity.y,
			//	m_xmf3Velocity.z*(cos(rand()) +	sin(rand()))*0.5);
			//
			//m_vParticles.emplace_back(Particle(m_xmf3Position, vel, m_fParticleLife));
			CreateParticles();
		}
	}
	
}
void ParticleSystem::FixedUpdate(float fTimeElapsed) {}

void ParticleSystem::BuildResource(shared_ptr<CreateManager> pCreateManager)
{

	m_pd3dUbParticles = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		sizeof(Particle) * m_uMaxSize, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다. 
	//m_pd3dcbParticles1->Map(0, NULL, (void **)m_vParticles.data());

	ChangeResourceState(m_pd3dCommandList.Get(), m_pd3dUbParticles, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//버퍼는 세개 만든다 하나는 uav , upload, readback
	//업데이트 전에 업로드힙에서 오브젝트 정보를 업데이트하고 uav로 복사, 업데이트 실행후 uav의 내용을 readBack으로
	//복사, -> readback에서 map함수호출로 최종값 불러옴.
	m_pd3dSrbParticles = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		sizeof(Particle) * m_uMaxSize, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
	m_pd3dSrbParticles->Map(0, NULL, (void **)&m_pSrbMappedParticles);
	memcpy(m_pSrbMappedParticles, m_vParticles.data(), sizeof(Particle) * m_uMaxSize);

	m_pd3dReadBackParticles = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		sizeof(Particle) * m_uMaxSize, D3D12_HEAP_TYPE_READBACK,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL);
	//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다. 
	m_pd3dCommandList->CopyResource(m_pd3dReadBackParticles, m_pd3dSrbParticles);

	UINT ncbElementBytes = ((sizeof(CB_Particle) + 255) & ~255); //256의 배수
	m_pd3dcbStruct = ::CreateBufferResource(pCreateManager->GetDevice().Get(), m_pd3dCommandList.Get(), NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbStruct->Map(0, NULL, (void **)&particleCb);
}

void ParticleSystem::ChangeResourceState(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER resourceBarrier;
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = pResource;
	resourceBarrier.Transition.StateBefore = stateBefore;
	resourceBarrier.Transition.StateAfter = stateAfter;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pCommandList->ResourceBarrier(1, &resourceBarrier);
}

void ParticleSystem::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
	if (m_pMaterial)
	{
		m_pMaterial->ReleaseUploadBuffers();
	}
}

void ParticleSystem::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	m_pd3dCommandList->SetGraphicsRootShaderResourceView(3,
		m_pd3dSrbParticles->GetGPUVirtualAddress());

	if (m_pShader)
		m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMaterial)
		m_pMaterial->UpdateShaderVariable(pd3dCommandList);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0, m_vParticles.size());
}