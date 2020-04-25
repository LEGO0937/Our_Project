#include "ParticleSystem.h"
#include <time.h>


ParticleSystem::ParticleSystem(shared_ptr<CreateManager> pCreateManager, const char& cPattern, const char& cShape, UINT uSize,
	CGameObject* pTarget, const XMFLOAT3& xmf3Position, const float& fVelocity,
	string pTextureName, const float& fLife): m_cPattern(cPattern),m_cShape(cShape), m_uSize(uSize),
	m_fParticleLife(fLife), m_fVelocity(fVelocity)
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
	CreateParticles();


	m_pTarget = pTarget;

	if (m_pTarget)
	{
		m_pTarget->AddRef();
		m_xmf3Position = m_pTarget->GetPosition();
	}
	else
		m_xmf3Position = xmf3Position;

	BuildResource(pCreateManager);
}
ParticleSystem::~ParticleSystem()
{
	if (m_pTarget)
		m_pTarget->Release();

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
	if (m_pd3dcbParticles1)
	{
		//m_pd3dcbParticles1->Unmap(0, NULL);
		m_pd3dcbParticles1->Release();
		m_pd3dcbParticles1 = NULL;
	}

	if (m_pd3dcbParticles2)
	{
		m_pd3dcbParticles2->Unmap(0, NULL);
		m_pd3dcbParticles2->Release();
		m_pd3dcbParticles2 = NULL;
	}
	if (m_pd3dcbParticles3)
	{
		m_pd3dcbParticles3->Unmap(0, NULL);
		m_pd3dcbParticles3->Release();
		m_pd3dcbParticles3 = NULL;
	}
	//m_vParticles.clear();
}

void ParticleSystem::AnimateObjects(float fTimeElapsed) 
{
	D3D12_RANGE readbackBufferRange{ 0,sizeof(Particle) * m_vParticles.size() };
	m_pd3dcbParticles2->Map(0, &readbackBufferRange, (void **)&m_pcbMappedParticles2);
	memcpy(m_pcbMappedParticles3, m_pcbMappedParticles2, sizeof(Particle) * m_vParticles.size());
	memcpy(m_vParticles.data(), m_pcbMappedParticles3, sizeof(Particle) * m_vParticles.size());
	m_pd3dcbParticles2->Unmap(0, NULL);

	if (m_bEnable)
	{
		if (curNumParticle + m_cShape > m_uMaxSize)
		{
			if (m_cPattern == ONES)
			{

				if (m_vParticles.size() == 0)
				{
				}//�޸� �ݳ�. 
			}
			else if (m_cPattern == LOOP)
			{
				curNumParticle = 0;
			}
		}
		else
		{
			Update(fTimeElapsed);
		}
	}
	memcpy(m_pcbMappedParticles3, m_vParticles.data(), sizeof(Particle) * m_uMaxSize);
	ChangeResourceState(m_pd3dCommandList.Get(), m_pd3dcbParticles1, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	m_pd3dCommandList->SetComputeRootShaderResourceView(4,
		m_pd3dcbParticles3->GetGPUVirtualAddress());
	m_pd3dCommandList->SetComputeRootUnorderedAccessView(5,
		m_pd3dcbParticles1->GetGPUVirtualAddress());

	m_pd3dCommandList->Dispatch(1, 1, 1);

	ChangeResourceState(m_pd3dCommandList.Get(), m_pd3dcbParticles1, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	m_pd3dCommandList->CopyResource(m_pd3dcbParticles2, m_pd3dcbParticles1);
}
void ParticleSystem::CreateParticles()
{

	if (m_pTarget)
	{
		//Ÿ���� ������Ŀ� �����°�(position)�� translate ��İ��� ����� �����ǰ��� ���ؿ´�.
		XMFLOAT4X4 matrix = Matrix4x4::Multiply(m_pTarget->m_xmf4x4World,
			XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z));
		m_xmf3Position.x = matrix._41; m_xmf3Position.y = matrix._42; m_xmf3Position.z = matrix._43;

		m_xmf3Position.x = (((float)rand() - (float)rand()) / RAND_MAX) * m_xmf3Position.x;
		m_xmf3Position.y = (((float)rand() - (float)rand()) / RAND_MAX) * m_xmf3Position.y;
		m_xmf3Position.z = (((float)rand() - (float)rand()) / RAND_MAX) * m_xmf3Position.z;
	}

	//XMFLOAT3 vel = XMFLOAT3(m_xmf3Velocity.x* cos(rand()), m_xmf3Velocity.y,
	//	m_xmf3Velocity.z*(cos(rand()) + sin(rand()))*0.5);

	//m_vParticles.emplace_back(Particle(m_xmf3Position, vel, m_fParticleLife));

	switch (m_cShape)
	{
	case CONE:
		if (curNumParticle + 15 < m_uMaxSize)
		{
			XMFLOAT3 vel = XMFLOAT3(0.5*m_fVelocity, 1 * m_fVelocity, 0);
			XMFLOAT3 xmf3Up = XMFLOAT3(0, 1, 0);
			XMVECTOR up = XMLoadFloat3(&xmf3Up);

			XMMATRIX mat = XMMatrixRotationAxis(up, XMConvertToRadians(24));
			for (int i = 0; i < 15; ++i)
			{
				m_vParticles.emplace_back(Particle(m_xmf3Position, vel, m_fParticleLife));
				vel = Vector3::TransformNormal(vel, mat);
			}
			curNumParticle += 15;
		}
		break;
	case RAND:
		if (curNumParticle < m_uMaxSize)
		{
			XMFLOAT3 vel = XMFLOAT3(m_fVelocity*cos(rand()), m_fVelocity,
				m_fVelocity * (cos(rand()) + sin(rand()))*0.5);

			m_vParticles.emplace_back(Particle(m_xmf3Position, vel, m_fParticleLife));
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
			//	//Ÿ���� ������Ŀ� �����°�(position)�� translate ��İ��� ����� �����ǰ��� ���ؿ´�.
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
		curNumParticle++;
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
	
}
void ParticleSystem::FixedUpdate(float fTimeElapsed) {}

void ParticleSystem::BuildResource(shared_ptr<CreateManager> pCreateManager)
{

	m_pd3dcbParticles1 = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		sizeof(Particle) * m_uMaxSize, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	//���� ����(���ε� ��)�� ���� �����͸� �����Ѵ�. 
	//m_pd3dcbParticles1->Map(0, NULL, (void **)m_vParticles.data());

	ChangeResourceState(m_pd3dCommandList.Get(), m_pd3dcbParticles1, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//���۴� ���� ����� �ϳ��� uav , upload, readback
	//������Ʈ ���� ���ε������� ������Ʈ ������ ������Ʈ�ϰ� uav�� ����, ������Ʈ ������ uav�� ������ readBack����
	//����, -> readback���� map�Լ�ȣ��� ������ �ҷ���.
	m_pd3dcbParticles3 = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		sizeof(Particle) * m_uMaxSize, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
	m_pd3dcbParticles3->Map(0, NULL, (void **)&m_pcbMappedParticles3);
	memcpy(m_pcbMappedParticles3, m_vParticles.data(), sizeof(Particle) * m_uMaxSize);

	m_pd3dcbParticles2 = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
		sizeof(Particle) * m_uMaxSize, D3D12_HEAP_TYPE_READBACK,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL);
	//���� ����(���ε� ��)�� ���� �����͸� �����Ѵ�. 
	m_pd3dCommandList->CopyResource(m_pd3dcbParticles2, m_pd3dcbParticles3);

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
		m_pd3dcbParticles3->GetGPUVirtualAddress());

	if (m_pShader)
		m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMaterial)
		m_pMaterial->UpdateShaderVariable(pd3dCommandList);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList, 0,m_vParticles.size());
}