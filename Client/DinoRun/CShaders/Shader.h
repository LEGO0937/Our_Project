#pragma once
#include "../Objects/BaseObject.h"
#include "../Objects/PlayerObject.h"
#include "../Common/Camera/Camera.h"

class CreateManager;

struct CB_BillBoard
{
	float fSize;
};

struct MODEL_INFO
{
	const char * modelName;
	const char * dataFileName;
	LPVOID updatedContext = NULL;
	bool useBillBoard = true;
	int size = 0;
};

class CShader
{
public:
	CShader();
	virtual ~CShader();

protected:
	int									m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual void CreateShaderVariables(CreateManager* pCreateManager) {};
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) {};
	virtual void ReleaseShaderVariables() {};

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState = 0);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) {}

	virtual void ReleaseUploadBuffers() { }

	virtual void BuildObjects(CreateManager* pCreateManager, void *pInformation = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed) { }
	virtual void ReleaseObjects() { }

	void CreateCbvSrvDescriptorHeaps(CreateManager* pCreateManager, int nConstantBufferViews, int nShaderResourceViews);
	void BackDescriptorHeapCount();

	D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferViews(CreateManager* pCreateManager, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(CreateManager* pCreateManager, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement);
	D3D12_GPU_DESCRIPTOR_HANDLE CreateShadowResourceViews(CreateManager* pCreateManager, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement);
	
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorNextHandle() { return(m_d3dCbvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_d3dCbvGPUDescriptorNextHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorNextHandle() { return(m_d3dSrvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorNextHandle() { return(m_d3dSrvGPUDescriptorNextHandle); }
protected:
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList = NULL;

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
protected:
	bool isEnable = true;          //그리기 유무
	bool isFixedUpdate = false;	   // 물리 처리 할것인지
	int drawingCount = 0;         // after culling, instancing number
	int drawingBillBoardCount = 0;         // after culling, instancing BillBoard's number

	string instancingModelName;   // model's name for instancing
	
	ID3D12Resource *m_pd3dcbStruct = NULL;
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation) = 0;
	virtual void ReleaseObjects() = 0;
	virtual void ReleaseUploadBuffers();

	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Update(float fTimeElapsed) {}
	virtual void FixedUpdate(float fTimeElapsed) {}
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT3& xmf3Position) {}

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) = 0;

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) {};
#ifdef _WITH_BOUND_BOX
	virtual void BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera) {};
#endif
	vector<CGameObject*>& getList()
	{
		return objectList;
	}

protected:
	CGameObject *m_ppObjects = NULL;   //인스턴싱drawing 대상
	vector<CGameObject*> objectList;   // 인스턴싱될 오브젝트들
	CGameObject *m_pBillBoardObject = NULL;   //인스턴싱drawing 빌보드 대상

	LPVOID						m_pUpdatedContext = NULL; //다루는 오브젝트들의 업데이트에 사용될 요소 주로 Terrain 사용
};

class CObInstancingShader : public CObjectsShader
{
public:
	CObInstancingShader();
	virtual ~CObInstancingShader();


	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ReleaseShaderVariables();

	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation) {}

	virtual void ReleaseObjects();
	
	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void FixedUpdate(float fTimeElapsed);
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition, const XMFLOAT4X4& xmf3ArrivePosition) {}

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void BillBoardRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
#ifdef _WITH_BOUND_BOX
	virtual void BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
#endif
	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL) {}
	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL) {}
protected:
	unordered_map<string, CB_OBJECT_INFO*> instancedObjectInfo;
	unordered_map<string, CB_OBJECT_INFO*> instancedBillBoardObjectInfo;

	unordered_map<string, CB_OBJECT_INFO*> instancedPrevObjectInfo;
	unordered_map<string, CB_OBJECT_INFO*> instancedPrevBillBoardObjectInfo;
};


class CSkinedObInstancingShader : public CObjectsShader
{
public:
	CSkinedObInstancingShader();
	virtual ~CSkinedObInstancingShader();


	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ReleaseShaderVariables();

	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation) {}

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Update(float fTimeElapsed) {}
	virtual void FixedUpdate(float fTimeElapsed) {}

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
#ifdef _WITH_BOUND_BOX
	virtual void BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
#endif
	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL) {}
	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL) {}

	vector<CGameObject*>& getSkiendList()
	{
		return objectList;
	}
protected:
	CPlayer *m_ppSkinedObjects = NULL;   //인스턴싱drawing 대상 SkinedObject로 바꿀 예정.
	vector<CPlayer*> m_vSkinedObjectList;   // 인스턴싱될 오브젝트들
	unordered_map<string, CB_SKINEOBJECT_INFO*> instancedObjectInfo;
	unordered_map<string, CB_SKINEOBJECT_INFO*> instancedPrevObjectInfo;
};


class CUiShader : public CObjectsShader
{
protected:
public:
	CUiShader();
	virtual ~CUiShader();

	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
	virtual void ReleaseObjects();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation) {}
	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ReleaseShaderVariables();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	vector<float>& getUvXs() { return uvX; }
protected:
	CB_UI_INFO *m_pcbMappedUis = NULL;
	ID3D12Resource *m_pd3dcbUis = NULL;
	vector<float> uvX;      //it is uv.x's  offsets
	vector<float> uvY;		//it is uv.y's  offsets
};


