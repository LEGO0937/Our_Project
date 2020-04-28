#include "ParticleSystem.h"
#include <time.h>


ParticleSystem::ParticleSystem(shared_ptr<CreateManager> pCreateManager, const char& cPattern, 
	const char& cShape, const float& fGravity,	const float& fSize,	CGameObject* pTarget,
	const XMFLOAT3& xmf3Position, const float& fVelocity,	string pTextureName, 
	const float& fLife, const UINT& uMaxSize): m_cPattern(cPattern),m_cShape(cShape), m_fSize(fSize),
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
	m_vParticles.clear();
}

bool ParticleSystem::AnimateObjects(float fTimeElapsed) 
{
	D3D12_RANGE readbackBufferRange{ 0,sizeof(Particle) * m_vParticles.size() };
	m_pd3dReadBackParticles->Map(0, &readbackBufferRange, (void **)&m_pReadBackMappedParticles);
	memcpy(m_pSrbMappedParticles, m_pReadBackMappedParticles, sizeof(Particle) * m_vParticles.size());
	memcpy(m_vParticles.data(), m_pSrbMappedParticles, sizeof(Particle) * m_vParticles.size());
	m_pd3dReadBackParticles->Unmap(0, NULL);
	m_pReadBackMappedParticles = NULL;

	XMFLOAT3 pos;
	if (m_pTarget)
	{
		pos = m_pTarget->GetPosition();
		pos = Vector3::Add(pos, m_pTarget->GetLook(), -m_xmf3Position.z);
	}
	else
		pos = m_xmf3Position;

	particleCb[0].xmf3Position = pos;
	particleCb[0].fElapsedTime = fTimeElapsed;
	particleCb[0].fGravity = m_fGravity;
	particleCb[0].fSize = m_fSize;

	if (m_bEnable)
	{
		if (curNumParticle + m_cShape > m_uMaxSize)
		{
			if (m_cPattern == ONES)
			{

				if (m_vParticles.size() == 0)
				{
					return true; //�޸� �ݳ�   ������ release ó��
				}
			}
			else if (m_cPattern == LOOP)
			{
				curNumParticle = 0;  //������ �����ڰ� ���� release�ؾ߸� ��
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

	XMFLOAT3 pos= XMFLOAT3(0.0f,0.0f,0.0f);


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
			auto time = std::chrono::system_clock::now();
			auto duration = time.time_since_epoch();     
			auto randomSeed = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

			std::mt19937 mtRand(randomSeed);                  
			std::uniform_real_distribution<double> randX(-1.0f, 1.0f);
			std::uniform_real_distribution<double> randY(0.1f, 2.0f);
			std::uniform_real_distribution<double> randZ(-1.0f, 0.1f);


			XMFLOAT3 vel = XMFLOAT3(m_fVelocity*randX(mtRand), m_fVelocity * randY(mtRand),
				m_fVelocity * randZ(mtRand));

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
	coolTime += fTimeElapsed;
	
	if (coolTime > 0.05)
	{
		coolTime = 0.f;
		if (m_vParticles.size() < m_uMaxSize)
			CreateParticles();
	}
	
}
void ParticleSystem::FixedUpdate(float fTimeElapsed) {}

void ParticleSystem::BuildResource(shared_ptr<CreateManager> pCreateManager)
{

	m_pd3dUbParticles = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		sizeof(Particle) * m_uMaxSize, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	//���� ����(���ε� ��)�� ���� �����͸� �����Ѵ�. 
	//m_pd3dcbParticles1->Map(0, NULL, (void **)m_vParticles.data());

	ChangeResourceState(m_pd3dCommandList.Get(), m_pd3dUbParticles, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//���۴� ���� ����� �ϳ��� uav , upload, readback
	//������Ʈ ���� ���ε������� ������Ʈ ������ ������Ʈ�ϰ� uav�� ����, ������Ʈ ������ uav�� ������ readBack����
	//����, -> readback���� map�Լ�ȣ��� ������ �ҷ���.
	m_pd3dSrbParticles = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		sizeof(Particle) * m_uMaxSize, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
	m_pd3dSrbParticles->Map(0, NULL, (void **)&m_pSrbMappedParticles);
	memcpy(m_pSrbMappedParticles, m_vParticles.data(), sizeof(Particle) * m_uMaxSize);

	m_pd3dReadBackParticles = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		sizeof(Particle) * m_uMaxSize, D3D12_HEAP_TYPE_READBACK,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL);
	//���� ����(���ε� ��)�� ���� �����͸� �����Ѵ�. 
	m_pd3dCommandList->CopyResource(m_pd3dReadBackParticles, m_pd3dSrbParticles);

	UINT ncbElementBytes = ((sizeof(CB_Particle) + 255) & ~255); //256�� ���
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
	pd3dCommandList->SetGraphicsRoot32BitConstants(2, 6, particleCb, 0);

	if (m_pShader)
		m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMaterial)
		m_pMaterial->UpdateShaderVariable(pd3dCommandList);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0, m_vParticles.size());
}