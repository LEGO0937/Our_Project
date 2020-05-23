//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "Camera.h"

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

class CShader;

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	UINT							m_nObjectID;						
};

struct CB_TEXTURE_MATRIX
{
	XMFLOAT4X4						m_xmf4x4TextureTransform;
};

//인스턴스 정보(게임 객체의 월드 변환 행렬과 객체의 생성)을 위한 구조이다.
struct VS_VB_INSTANCE
{
	XMFLOAT4X4 m_xmf4x4Transform;
	//XMFLOAT4 m_xmColor;
	//XMFLOAT2 m_xmTexcoord;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//struct CB_TEXTURE_COORD
//{
//	XMFLOAT4X4						m_xmf4x4Texcoord;
//};

class CTexture
{
public:
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;
	int								m_nTextures = 0;	//텍스쳐도 배열의 원소의 개수만큼 가질 수 있다.
	ID3D12Resource					**m_ppd3dTextures = NULL;
	ID3D12Resource					**m_ppd3dTextureUploadBuffers;

	SRVROOTARGUMENTINFO				*m_pRootArgumentInfos = NULL;	//뷰가 몇번째 루트 시그너쳐에 있는 파라미터인지 , 인덱스와 디스크립터핸들. 이정보가 있으면 셋을 할수 있다.

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex);

	int GetTextures() { return(m_nTextures); }
	ID3D12Resource *GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	UINT GetTextureType() { return(m_nTextureType); }

	void ReleaseUploadBuffers();
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	//기본 색상
	XMFLOAT4						m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


	CTexture						*m_pTexture = NULL;
	CShader							*m_pShader = NULL;

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetTexture(CTexture *pTexture);
	void SetShader(CShader *pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
public:
	CGameObject(int nMeshes=1);
	virtual ~CGameObject();

public:


	XMFLOAT4X4						m_xmf4x4World;

	XMFLOAT4X4						m_xmf4x4Texcoord;			//텍스쳐 변환 행렬

	//이제 메시마다 재질을 다르게 해야하기 때문에 subset을 다르게 해야하는 재질의 개수만큼 subset이 필요하다
	//배치 프로세싱 개념
	//subset의 개념은 같은 재질을 사용해서 그려야할것 집합해놓은것
	CMesh							**m_ppMeshes;
	int								m_nMeshes;

	CMaterial						*m_pMaterial = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	BoundingOrientedBox m_xmOOBB;
	BoundingOrientedBox m_xmOOBBTransformed;

	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) {
		m_xmOOBBTransformed = m_xmOOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
	}

protected:
	ID3D12Resource					*m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO				*m_pcbMappedGameObject = NULL;

	ID3D12Resource					*m_pd3dcbTexture = NULL;
	CB_TEXTURE_MATRIX				*m_pcbMappedTextureMatrix = NULL;


	UINT							m_terrainID;
public:
	//게임 객체가 보이는가를 결정한다.
	bool IsVisible(CCamera* pCamera = NULL);
	bool IsVisible(int nIndex,CCamera* pCamera = NULL);

	void SetMesh(int nIndex, CMesh *pMesh);
	void SetShader(CShader *pShader);
	void SetMaterial(CMaterial *pMaterial);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Animate(float fTimeElapsed);
	virtual void Animate(float fTimeElapsed,CCamera *pCamera, bool bIntersect){}
	virtual void Animate(float fTimeElapsed,CCamera *pCamera, CPlayer *pPlayer= NULL){}
	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera,int objNum);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT nInstance,int objNum);
	virtual void RenderInstancing(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);

	virtual void BuildMaterials(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes=1);
	virtual ~CRotatingObject();

private:
	XMFLOAT3						m_xmf3RotationAxis;
	float							m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera,int objNum);
};

class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject(int nMeshes=1);
	virtual ~CRevolvingObject();

private:
	XMFLOAT3						m_xmf3RevolutionAxis;
	float							m_fRevolutionSpeed;

public:
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	void SetRevolutionAxis(XMFLOAT3 xmf3RevolutionAxis) { m_xmf3RevolutionAxis = xmf3RevolutionAxis; }

	virtual void Animate(float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color,UINT id);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage					*m_pHeightMapImage;

	int								m_nWidth;
	int								m_nLength;

	XMFLOAT3						m_xmf3Scale;
	//UINT							m_terrainID;
public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }


	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
};

class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRoogSignature);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

};