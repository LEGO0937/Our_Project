#pragma once
#include "../../CShaders/Shader.h"
#include "../../Meshes/BillBoardMesh.h"
#include "../../Common/FrameWork/CreateManager.h"

#include "../stdafx.h"
#define LOOP 0
#define ONES 1

#define RAND 1
#define CONE 15
#define BOOM 2



struct CB_Particle
{
	XMFLOAT4X4 m_xmf4x4World;
};
class Particle
{
public:
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0, 0, 0);
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	float life = 0;

	Particle(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLife) :m_xmf3Position(xmf3Position),
		m_xmf3Velocity(xmf3Velocity), life(fLife) {}

};
class ParticleSystem
{
private:
	bool m_bEnable = true;

	CMesh* m_pMesh = NULL;
	char m_cPattern = ONES;
	char m_cShape = CONE;

	CMaterial* m_pMaterial = NULL;
	CShader* m_pShader = NULL;

	UINT m_uSize = 0;
	UINT m_uMaxSize = 1000;

	XMFLOAT3 m_xmf3Position;  //target이 없으면 고정 위치값, 있으면 target의 포지션에 대한 offset값
	float m_fVelocity = 1;

	float coolTime = 0.0f;
	float curNumParticle = 0;

	CGameObject* m_pTarget = NULL;
	vector<Particle> m_vParticles;
	float m_fParticleLife = 0;

	Particle *m_pcbMappedParticles1 = NULL;
	ID3D12Resource *m_pd3dcbParticles1 = NULL;

	Particle *m_pcbMappedParticles2 = NULL;
	ID3D12Resource *m_pd3dcbParticles2 = NULL;

	Particle *m_pcbMappedParticles3 = NULL;
	ID3D12Resource *m_pd3dcbParticles3 = NULL;

	
	int m_iCurrentBufferIndex = 0;

	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList = NULL;

	
public:
	ParticleSystem(shared_ptr<CreateManager> pCreateManager, const char& cPattern, const char& cShape, UINT uSize, CGameObject* pTarget, const XMFLOAT3& xmf3Position,
		 const float& fVelocity,
		string pTextureName, const float& fLife);
	~ParticleSystem();
	vector<Particle> GetParticles() { return m_vParticles; }
	void AnimateObjects(float fTimeElapsed);
	void Update(float fTimeElapsed);
	void FixedUpdate(float fTimeElapsed);

	void BuildResource(shared_ptr<CreateManager> pCreateManager);
	void ReleaseUploadBuffers();
	void ChangeResourceState(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	void CreateParticles();
	void SetEnable(bool enable) { m_bEnable = enable; }
};

