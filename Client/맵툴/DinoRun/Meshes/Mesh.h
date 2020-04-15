//------------------------------------------------------- ----------------------
// File: Mesh.h
//-----------------------------------------------------------------------------

#pragma once
#include "../Common/stdafx.h"
class CGameObject;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define VERTEXT_POSITION				0x0001
#define VERTEXT_COLOR					0x0002
#define VERTEXT_NORMAL					0x0004
#define VERTEXT_TANGENT					0x0008
#define VERTEXT_TEXTURE_COORD0			0x0016
#define VERTEXT_TEXTURE_COORD1			0x0032

#define VERTEXT_BONE_INDEX_WEIGHT		0x1000

#define VERTEXT_DEFAULT					(VERTEXT_POSITION | VERTEXT_NORMAL)
#define VERTEXT_TEXTURE					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_DETAIL					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TEXTURE			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_TANGENT_TEXTURE	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_DETAIL			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TANGENT__DETAIL	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMesh
{
public:
	CMesh() {}
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CMesh();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	char							m_pstrMeshName[64] = { 0 };

protected:
	UINT							m_nType = 0x00;

	XMFLOAT3						m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3						m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nOffset = 0;

protected:
	int								m_nVertices = 0;

	XMFLOAT3						*m_pxmf3Positions = NULL;
	XMFLOAT3						*m_pxmf3Normals = NULL;
	XMFLOAT2						*m_pxmf2Uvs = NULL;
	XMFLOAT2						*m_pxmf2DetailedUvs = NULL;

	ID3D12Resource					*m_pd3dPositionBuffer = NULL;
	ID3D12Resource					*m_pd3dPositionUploadBuffer = NULL;

	ID3D12Resource					*m_pd3dUvBuffer = NULL;
	ID3D12Resource					*m_pd3dUvUploadBuffer = NULL;

	ID3D12Resource					*m_pd3dDetailedUvBuffer = NULL;
	ID3D12Resource					*m_pd3dDetailedUvUploadBuffer = NULL;

	ID3D12Resource					*m_pd3dNormalBuffer = NULL;
	ID3D12Resource					*m_pd3dNormalUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dUvBufferView;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dDetailedUvBufferView;

	int								m_nSubMeshes = 0;
	int								*m_pnSubSetIndices = NULL;
	UINT							**m_ppnSubSetIndices = NULL;

	ID3D12Resource					**m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource					**m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW			*m_pd3dSubSetIndexBufferViews = NULL;

public:
	UINT GetType() { return(m_nType); }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet, int nInstance);
	virtual void OnPostRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext);

	void LoadMeshFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FILE *pInFile);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define SKINNED_ANIMATION_BONES		128

class CSkinnedMesh : public CMesh
{
public:
	CSkinnedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CSkinnedMesh();

protected:
	int								m_nBonesPerVertex = 4;

	XMINT4							*m_pxmn4BoneIndices = NULL;

	ID3D12Resource					*m_pd3dBoneIndexBuffer = NULL;
	ID3D12Resource					*m_pd3dBoneIndexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneIndexBufferView;

	XMFLOAT4						*m_pxmf4BoneWeights = NULL;

	ID3D12Resource					*m_pd3dBoneWeightBuffer = NULL;
	ID3D12Resource					*m_pd3dBoneWeightUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneWeightBufferView;

public:
	int								m_nSkinningBones = 0; 

	char							(*m_ppstrSkinningBoneNames)[64];
	CGameObject						**m_ppSkinningBoneFrameCaches = NULL;

	XMFLOAT4X4						*m_pxmf4x4BindPoseBoneOffsets = NULL; 

	ID3D12Resource					*m_pd3dcbBindPoseBoneOffsets = NULL;
	XMFLOAT4X4						*m_pcbxmf4x4MappedBindPoseBoneOffsets = NULL; //Transposed

	ID3D12Resource					*m_pd3dcbSkinningBoneTransforms = NULL;
	XMFLOAT4X4						*m_pcbxmf4x4MappedSkinningBoneTransforms = NULL;

public:
	void PrepareSkinning(CGameObject *pModelRootObject);
	void LoadSkinDeformationsFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FILE *pInFile);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext);
};