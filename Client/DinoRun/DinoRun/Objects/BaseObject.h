#pragma once
#include "../Common/stdafx.h"
#include "../Meshes/Mesh.h"

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05
#define MAX_BON_NUM 10

class CShader;
class CCamera;
class CreateManager;
class ParticleSystem; 
class CAnimationSets;
class CAnimationController;

struct CB_OBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4PrevWorld;
};

struct CB_SKINEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4Worlds[MAX_BON_NUM];
	XMFLOAT4X4 m_xmf4x4PrevWorlds[MAX_BON_NUM];
};

struct CB_UI_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	float uvX = 0;
	float uvY = 0;
};

struct SRVROOTARGUMENTINFO
{
	int								m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};



class CTexture
{
public:
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;

	int								m_nTextures = 0;
	ID3D12Resource					**m_ppd3dTextures = NULL;
	ID3D12Resource					**m_ppd3dTextureUploadBuffers = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSamplerGpuDescriptorHandles = NULL;

	SRVROOTARGUMENTINFO				*m_pRootArgumentInfos = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, UINT nIndex);
	void SetTexture(ComPtr<ID3D12Resource> resouce, UINT nIndex);

	int GetTextures() { return(m_nTextures); }
	ID3D12Resource *GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	UINT GetTextureType() { return(m_nTextureType); }

	void ReleaseUploadBuffers();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct MATERIAL
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive;
};


class CMaterial
{
public:
	CMaterial(int nTextures);
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

private:
	ID3D12Resource *m_pd3dcbMaterials = NULL;
	MATERIAL *m_pcbMappedMaterials = NULL;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CShader							*m_pShader = NULL;

	XMFLOAT4						m_xmf4DiffuseColor = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	XMFLOAT4						m_xmf4AmbientColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;


	void SetShader(CShader *pShader);
	void SetTexture(CTexture *pTexture, UINT nTexture = 0);

	virtual void CreateShaderVariable(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void ReleaseUploadBuffers();

public:
	int 							m_nTextures = 0;
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
	CTexture						**m_ppTextures = NULL; //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR *pwstrTextureName, CTexture **ppTexture, CGameObject *pParent, FILE *pInFile, CShader *pShader) {};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


class CLoadedModelInfo
{
public:
	CLoadedModelInfo() { }
	~CLoadedModelInfo();

public:
	CGameObject						*m_pModelRootObject = NULL;

	CAnimationSets					*m_pAnimationSets = NULL;

	int 							m_nSkinnedMeshes = 0;
	CSkinnedMesh					**m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

public:
	void PrepareSkinning();
};


class RigidBody
{
private:
	BodyType m_bodyType;

};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
protected:
	int								m_nReferences = 0;
	float m_fMaxForce = MAX_FORCE; // 힘의 크기 이 이상 넘을 경우 이 값을 유지함 N단위

	float m_nScale = 1;

	CB_OBJECT_INFO *m_pcbMappedGameObject = NULL;  //no Skined Object's instancing buffer
	ID3D12Resource *m_pd3dcbGameObject = NULL;

	CB_OBJECT_INFO *m_pcbMappedGameObjects = NULL;  //no Skined Object's instancing buffer
	ID3D12Resource *m_pd3dcbGameObjects = NULL;

	CB_SKINEOBJECT_INFO *m_pcbMappedSkinedGameObjects = NULL; // Skined Object's instancing buffer
	ID3D12Resource *m_pd3dcbSkinedGameObjects = NULL;
public:
	void AddRef();
	void Release();

	CB_OBJECT_INFO *m_pcbMappedBillBoardObjects = NULL;  //BillBoard Object's instancing buffer
	ID3D12Resource *m_pd3dcbBillBoardObjects = NULL;

public:
	CGameObject();
	CGameObject(int nMaterials);
	virtual ~CGameObject();
protected:
	bool							isSkined = false;  //스킨 매시를 사용하는지
	bool							isKinematic = false; //충돌 체크시 물리효과를 적용할 것인가 y or n 
	bool							isEnable = true;  //게임 상에 존재하게 할 것인지 y or n false이면 update,render X

	int								m_iSerealNum = 0;
	string							m_sObjectName = "None";

	ModelType						m_ModelType = ModelType::Default;   //Layer와 유사

	CMesh							*m_pMesh = NULL;

	int								m_nMaterials = 0;
	CMaterial						**m_ppMaterials = NULL;

	CGameObject 					*m_pParent = NULL;
	CGameObject 					*m_pChild = NULL;
	CGameObject 					*m_pSibling = NULL;

	LPVOID						m_pUpdatedContext = NULL;
public:
	int GetId() { return m_iSerealNum; }
	void SetId(const int& id) { m_iSerealNum = id; }

	string GetName() { return m_sObjectName; }
	void SetName(const string& id) { m_sObjectName = id; }

	CGameObject* GetChild() { return m_pChild; }

	void SetSkinedState(bool bIsSkined) { isSkined = bIsSkined; }
	
	void SetKinematicState(bool bIsKinematic) { isKinematic = bIsKinematic; }
	bool GetKinematicState() { return isKinematic; }

	void SetEnableState(bool bIsEnable) { isEnable = bIsEnable; }
	bool GetEnableState() { return isEnable; }

	ModelType GetModelType() { return m_ModelType; }
	char							m_pstrFrameName[64] = "RootNode";

	   //충돌체크시 사용될 오브젝트의 유형(player, wall 등등)
	//-----------------------rigidBody----------------------
	float							m_fMass = 0;   //kg 단위
	
	XMFLOAT3 m_xmf3Angle;
	XMFLOAT3 m_xmf3Forces = { 0,0,0 };  // 충돌 시 적용할 힘을 추가 하기 위한 변수.  N단위
	XMFLOAT3     				m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3 m_xmf3RotateVelocity =XMFLOAT3(0.0f,0.0f,0.0f);
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);  //속력
	XMFLOAT3 m_xmf3AcceleratingForce = { 0.0f,0.0f,0.0f };   //가속력

	float m_fForce = 0;    //앞키 누를 시 증가하는 변수, 룩벡터에 곱함으로써 진행 방향에 대한 힘벡터를 구함.



	//---------------------------------------------------

	XMFLOAT4X4						m_xmf4x4ToParent;
	XMFLOAT4X4						m_xmf4x4World;  //현재 프레임의 월드 행렬
	XMFLOAT4X4						m_xmf4x4PrevWorld;  //이전 프레임의 월드 행렬

	XMFLOAT3					m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	ParticleSystem* m_pParticleSystem = NULL;
public:

	void SetMesh(CMesh *pMesh);
	CMesh* GetMesh() { return m_pMesh; }
	void SetShader(CShader *pShader);
	void SetShader(int nMaterial, CShader *pShader);
	void SetMaterial(int nMaterial, CMaterial *pMaterial);

	virtual void resetShadowTexture(CreateManager* pCreateManager);

	void SetChild(CGameObject *pChild, bool bReferenceUpdate = false);
	void SetMatrix(const XMFLOAT4X4& xmf4x4Matrix) { m_xmf4x4ToParent = xmf4x4Matrix; }
	
	virtual void OnPrepareAnimate() { }
	virtual void Animate(float fTimeElapsed); // 애니메이션 처리
	virtual void FixedUpdate(float fTimeElapsed); //물리
	virtual bool Update(float fTimeElapsed, CGameObject* target) { return false; } //충돌처리
	virtual void UpdateDistance(float fTimeElapsed, CGameObject* target); //충돌시 거리 조절

	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT nInstances);
	virtual void ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT nInstances);
#ifdef _WITH_BOUND_BOX
	virtual void BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT nInstances);
#endif

	virtual void CreateShaderVariables(CreateManager* pCreateManager) {}
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) {}
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList);


	void CreateBuffer(CreateManager* pCreateManager);
	void CreateInstanceBuffer(CreateManager* pCreateManager,
		UINT nInstances, unordered_map<string, CB_OBJECT_INFO*>& uMap);
	void CreateBillBoardInstanceBuffer(CreateManager* pCreateManager,
		UINT nInstances);
	void CreateSkinedInstanceBuffer(CreateManager* pCreateManager,
		UINT nInstances, unordered_map<string, CB_SKINEOBJECT_INFO*>& uMap);

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }

	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4 *pxmf4Quaternion);

	CGameObject *GetParent() { return(m_pParent); }

	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent = NULL);
	void UpdateTransform_Instancing(unordered_map<string, CB_OBJECT_INFO*>& instancedTransformBuffer, const int& idx, XMFLOAT4X4 *pxmf4x4Parent);
	void UpdateTransform_BillBoardInstancing(CB_OBJECT_INFO* buffer, const int& idx, XMFLOAT4X4 *pxmf4x4Parent);
	void UpdateTransform_SkinedInstancing(unordered_map<string, CB_SKINEOBJECT_INFO*>& instancedTransformBuffer, const int& idx);

	CGameObject *FindFrame(char *pstrFrameName);

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0x00); }

	bool IsVisible(CCamera *pCamera);
	bool IsVisible_Ins(CCamera *pCamera);
	bool IsCollide(CGameObject* ob);
	bool IsCollide(CGameObject* a, CGameObject* b);

	void SetMaxForce(float fForce) { m_fMaxForce = fForce; }
	float GetMaxForce() { return m_fMaxForce; }

	void SetUpdatedContext(LPVOID pContext) { m_pUpdatedContext = pContext; }
	virtual void OnUpdateCallback(float fTimeElapsed);

	void AddForce(const XMFLOAT3& forces) { m_xmf3Forces = forces; }
public:
	CAnimationController 			*m_pSkinnedAnimationController = NULL;

	CSkinnedMesh *FindSkinnedMesh(char *pstrSkinnedMeshName);
	void FindAndSetSkinnedMesh(CSkinnedMesh **ppSkinnedMeshes, int *pnSkinnedMesh);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackAnimationPosition(int nAnimationTrack, float fPosition);

	void LoadMaterialsFromFile(CreateManager* pCreateManager, CGameObject *pParent, FILE *pInFile, CShader *pShader);

	static void LoadAnimationFromFile(FILE *pInFile, CLoadedModelInfo *pLoadedModel);
	static CGameObject *LoadFrameHierarchyFromFile(CreateManager* pCreateManager, CGameObject *pParent, FILE *pInFile, CShader *pShader, int *pnSkinnedMeshes);

	static CLoadedModelInfo *LoadGeometryAndAnimationFromFile(CreateManager* pCreateManager, const char *pstrFileName, CShader *pShader);

	static void PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

int ReadIntegerFromFile(FILE *pInFile);
float ReadFloatFromFile(FILE *pInFile);
int ReadStringFromFile(FILE *pInFile, char *pstrToken);