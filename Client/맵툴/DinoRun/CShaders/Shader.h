#pragma once
#include "../Objects/BaseObject.h"

class CShader
{
public:
	CShader();
	virtual ~CShader();

private:
	int									m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {};
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) {};
	virtual void ReleaseShaderVariables() {};

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World) { }

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState=0);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	virtual void ReleaseUploadBuffers() { }

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed) { }
	virtual void ReleaseObjects() { }

	 void CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);

	 D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	 D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(ID3D12Device *pd3dDevice, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorNextHandle() { return(m_d3dCbvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_d3dCbvGPUDescriptorNextHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorNextHandle() { return(m_d3dSrvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorNextHandle() { return(m_d3dSrvGPUDescriptorNextHandle); }
protected:
	float								m_fElapsedTime = 0.0f;

	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorNextHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;
};

class CObjectsShader : public CShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain) = 0;
	virtual void ReleaseObjects() = 0;
	virtual void ReleaseUploadBuffers();

	virtual void AnimateObjects(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent = NULL);
	virtual void AnimateObjects(CGameObject *pOb, float fTimeElapsed);


	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) = 0;

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	list<CGameObject*>& getList()
	{
		return objectList;
	}
protected:
	CB_OBJECT_INFO *m_pcbMappedGameObjects = NULL;
	ID3D12Resource *m_pd3dcbGameObjects = NULL;

	CGameObject *m_ppObjects = NULL;
	int m_nObjects = 0;
	list<CGameObject*> objectList;
};

class CObInstancingShader : public CObjectsShader
{
public:
	CObInstancingShader();
	virtual ~CObInstancingShader();


	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain) {}
	virtual void ReleaseObjects();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, 
		ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix) {}
	virtual void Delete() {}
	virtual void Save() {}
	virtual void Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) {}
protected:
	unordered_map<string, CB_OBJECT_INFO*> instancedObjectInfo;
};



class CObInstancingMaptoolShader : public CObjectsShader
{
public:
	CObInstancingMaptoolShader();
	virtual ~CObInstancingMaptoolShader();


	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain) = 0;
	virtual void ReleaseObjects();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

protected:
	unordered_map<string, CB_OBJECT_INFO*> instancedObjectInfos[1];
	CGameObject* m_ppObjectss[1];
	list<CGameObject*> objectLists[1];
	int nObject = 0;
};