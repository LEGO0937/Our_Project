//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------
#include "BaseObject.h"
#include "../Common/FrameWork/CreateManager.h"
#include "../Common/Camera/Camera.h"
#include "../CShaders/Shader.h"
#include "../Scenes/BaseScene.h"
#include "PlayerObject.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define COLLISION_LENGTH 50

CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_nTextureType = nTextureType;
	m_nTextures = nTextures;

	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppd3dTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppd3dTextures = new ID3D12Resource*[m_nTextures];
	}
	for (int i = 0; i < m_nTextures; ++i)
	{
		m_ppd3dTextureUploadBuffers[i] = NULL;
		m_ppd3dTextures[i] = NULL;
	}
	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}

	if (m_pRootArgumentInfos)
	{
		delete[] m_pRootArgumentInfos;
	}

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, &(m_ppd3dTextureUploadBuffers[nIndex]), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}
void CTexture::SetTexture(ComPtr<ID3D12Resource> resouce, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = resouce.Get();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial(int nTextures)
{
	m_nTextures = nTextures;

	m_ppTextures = new CTexture*[m_nTextures];
	m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
	for (int i = 0; i < m_nTextures; i++) m_ppTextures[i] = NULL;
	for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';
}

CMaterial::~CMaterial()
{
	if (m_pShader) m_pShader->Release();

	if (m_nTextures > 0)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppTextures[i]) m_ppTextures[i]->Release();
		delete[] m_ppTextures;

		if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;
	}
	if (m_pd3dcbMaterials)
	{
		m_pd3dcbMaterials->Unmap(0, NULL);
		m_pd3dcbMaterials->Release();
	}
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::SetTexture(CTexture *pTexture, UINT nTexture)
{
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->Release();
	m_ppTextures[nTexture] = pTexture;
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->AddRef();
}

void CMaterial::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->ReleaseUploadBuffers();
	}
}

void CMaterial::CreateShaderVariable(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	UINT ncbMaterialBytes = ((sizeof(MATERIAL) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbMaterials->Map(0, NULL, (void **)&m_pcbMappedMaterials);
}

void CMaterial::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pcbMappedMaterials->m_xmf4Ambient = m_xmf4AmbientColor;
	m_pcbMappedMaterials->m_xmf4Diffuse = m_xmf4DiffuseColor;
	m_pcbMappedMaterials->m_xmf4Emissive = m_xmf4EmissiveColor;
	m_pcbMappedMaterials->m_xmf4Specular = m_xmf4SpecularColor;

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress =
		m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4,
		d3dcbMaterialsGpuVirtualAddress);

	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->UpdateShaderVariable(pd3dCommandList, 0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationCurve::CAnimationCurve(int nKeys)
{
	m_nKeys = nKeys;
	m_pfKeyTimes = new float[nKeys];
	m_pfKeyValues = new float[nKeys];
}

CAnimationCurve::~CAnimationCurve()
{
	if (m_pfKeyTimes) delete[] m_pfKeyTimes;
	if (m_pfKeyValues) delete[] m_pfKeyValues;
}

float CAnimationCurve::GetValueByLinearInterpolation(float fPosition)
{
	for (int k = 0; k < (m_nKeys - 1); k++)
	{
		if ((m_pfKeyTimes[k] <= fPosition) && (fPosition <= m_pfKeyTimes[k + 1]))
		{
			float t = (fPosition - m_pfKeyTimes[k]) / (m_pfKeyTimes[k + 1] - m_pfKeyTimes[k]);
			return(m_pfKeyValues[k] * (1.0f - t) + m_pfKeyValues[k + 1] * t);
		}
	}
	return(m_pfKeyValues[m_nKeys - 1]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationLayer::CAnimationLayer()
{
}

CAnimationLayer::~CAnimationLayer()
{
	for (int i = 0; i < m_nAnimatedBoneFrames; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (m_ppAnimationCurves[i][j]) delete m_ppAnimationCurves[i][j];
		}
	}
	if (m_ppAnimationCurves) delete[] m_ppAnimationCurves;

	if (m_ppAnimatedBoneFrameCaches) delete[] m_ppAnimatedBoneFrameCaches;
}

void CAnimationLayer::LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE *pInFile)
{
	int nAnimationKeys = ::ReadIntegerFromFile(pInFile);

	m_ppAnimationCurves[nBoneFrame][nCurve] = new CAnimationCurve(nAnimationKeys);

	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyTimes, sizeof(float), nAnimationKeys, pInFile);
	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyValues, sizeof(float), nAnimationKeys, pInFile);
}

XMFLOAT4X4 CAnimationLayer::GetSRT(int nBoneFrame, float fPosition)
{
	XMFLOAT4X4 xmf4x4Transform;
	XMFLOAT3 xmf3S = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 xmf3R = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 xmf3T = XMFLOAT3(0.0f, 0.0f, 0.0f);

	if (m_ppAnimationCurves[nBoneFrame][0]) xmf3T.x = m_ppAnimationCurves[nBoneFrame][0]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][1]) xmf3T.y = m_ppAnimationCurves[nBoneFrame][1]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][2]) xmf3T.z = m_ppAnimationCurves[nBoneFrame][2]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][3]) xmf3R.x = m_ppAnimationCurves[nBoneFrame][3]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][4]) xmf3R.y = m_ppAnimationCurves[nBoneFrame][4]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][5]) xmf3R.z = m_ppAnimationCurves[nBoneFrame][5]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][6]) xmf3S.x = m_ppAnimationCurves[nBoneFrame][6]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][7]) xmf3S.y = m_ppAnimationCurves[nBoneFrame][7]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][8]) xmf3S.z = m_ppAnimationCurves[nBoneFrame][8]->GetValueByLinearInterpolation(fPosition);

	XMMATRIX R = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationX(xmf3R.x), XMMatrixRotationY(xmf3R.y)), XMMatrixRotationZ(xmf3R.z));
	XMMATRIX S = XMMatrixScaling(xmf3S.x, xmf3S.y, xmf3S.z);
	XMMATRIX T = XMMatrixTranslation(xmf3T.x, xmf3T.y, xmf3T.z);
	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixMultiply(XMMatrixMultiply(S, R), T));

	return(xmf4x4Transform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSet::CAnimationSet(float fStartTime, float fEndTime, char *pstrName)
{
	m_fStartTime = fStartTime;
	m_fEndTime = fEndTime;
	m_fLength = fEndTime - fStartTime;

	strcpy_s(m_pstrAnimationSetName, 64, pstrName);
}

CAnimationSet::~CAnimationSet()
{
	if (m_pAnimationLayers) delete[] m_pAnimationLayers;

	if (m_pCallbackKeys) delete[] m_pCallbackKeys;
	if (m_pCallbackFuncKeys) delete[] m_pCallbackFuncKeys;

	if (m_pAnimationCallbackHandler) delete m_pAnimationCallbackHandler;
	if (m_pAnimationCallbackFuncHandler) delete m_pAnimationCallbackFuncHandler;
}

void *CAnimationSet::GetCallbackData()
{
	for (int i = 0; i < m_nCallbackKeys; i++)
	{
		if (::IsEqual(m_pCallbackKeys[i].m_fTime, m_fPosition, ANIMATION_CALLBACK_EPSILON))
			return(m_pCallbackKeys[i].m_pCallbackData);
	}
	return(NULL);
}

void *CAnimationSet::GetCallbackFuncData(CALLBACKFUNCData& data)
{
	for (int i = 0; i < m_nCallbackFuncKeys; i++)
	{
		if (::IsEqual(m_pCallbackFuncKeys[i].m_fTime, m_fPosition, ANIMATION_CALLBACK_EPSILON))
		{
			data.m_pAnimationController = m_pCallbackFuncKeys[i].m_pAnimationController;
			data.m_pCallbackData = m_pCallbackFuncKeys[i].m_pCallbackData;
			data.m_pCallbackPrevData = m_pCallbackFuncKeys[i].m_pCallbackPrevData;
			return(m_pCallbackFuncKeys[i].m_pAnimationController);
		}
	}
	return(NULL);
}

void CAnimationSet::SetPosition(float fTrackPosition)
{
	m_fPosition = fTrackPosition;
	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		m_fPosition = m_fStartTime + ::fmod(fTrackPosition, m_fLength);
		break;
	}
	case ANIMATION_TYPE_ONCE:
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;
	}

	if (m_pAnimationCallbackHandler)
	{
		void *pCallbackData = GetCallbackData();
		if (pCallbackData) m_pAnimationCallbackHandler->HandleCallback(pCallbackData);
	}
	if (m_pCallbackFuncKeys)
	{
		CALLBACKFUNCData data;
		void *pCallbackData = GetCallbackFuncData(data);
		if (pCallbackData)
		{
			CAnimationController* controller = (CAnimationController*)data.m_pAnimationController;
			controller->SetTrackEnable(controller->m_CurrentTrack, false);
			controller->SetTrackEnable(data.m_pCallbackData, true);
			controller->SetTrackPosition(data.m_pCallbackData, 0.0f);
			controller->m_CurrentTrack = data.m_pCallbackData;

			//m_pAnimationCallbackFuncHandler->HandleCallback(&data);
		}
	}
}

void CAnimationSet::Animate(float fTrackPosition, float fTrackWeight)
{
	SetPosition(fTrackPosition);

	for (int i = 0; i < m_nAnimationLayers; i++)
	{
		for (int j = 0; j < m_pAnimationLayers[i].m_nAnimatedBoneFrames; j++)
		{
			m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = Matrix4x4::Zero();
		}
	}

	for (int i = 0; i < m_nAnimationLayers; i++)
	{
		for (int j = 0; j < m_pAnimationLayers[i].m_nAnimatedBoneFrames; j++)
		{
			XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Scale(m_pAnimationLayers[i].GetSRT(j, m_fPosition), m_pAnimationLayers[i].m_fWeight * fTrackWeight);
			m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = Matrix4x4::Add(xmf4x4Transform, m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent);
		}
	}
}

void CAnimationSet::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSet::SetCallbackFuncKeys(int nCallbackKeys)
{
	m_nCallbackFuncKeys = nCallbackKeys;
	m_pCallbackFuncKeys = new CALLBACKFUNCKEY[nCallbackKeys];
}

void CAnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void *pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}
void CAnimationSet::SetCallbackFuncKey(int nKeyIndex, float fKeyTime, void *pData1, int pData2, int pData3)
{
	m_pCallbackFuncKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackFuncKeys[nKeyIndex].m_pCallbackData = pData3;
	m_pCallbackFuncKeys[nKeyIndex].m_pCallbackPrevData = pData2;
	m_pCallbackFuncKeys[nKeyIndex].m_pAnimationController = pData1;
}

void CAnimationSet::SetAnimationCallbackHandler(CAnimationCallbackHandler *pCallbackHandler)
{
	m_pAnimationCallbackHandler = pCallbackHandler;
}

void CAnimationSet::SetAnimationCallbackFuncHandler(CAnimationCallbackHandler *pCallbackHandler)
{
	m_pAnimationCallbackFuncHandler = pCallbackHandler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSets::CAnimationSets(int nAnimationSets)
{
	m_nAnimationSets = nAnimationSets;
	m_ppAnimationSets = new CAnimationSet*[nAnimationSets];
}

CAnimationSets::~CAnimationSets()
{
	for (int i = 0; i < m_nAnimationSets; i++) if (m_ppAnimationSets[i]) delete m_ppAnimationSets[i];
	if (m_ppAnimationSets) delete[] m_ppAnimationSets;
}

void CAnimationSets::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	m_ppAnimationSets[nAnimationSet]->m_nCallbackKeys = nCallbackKeys;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSets::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void *pData)
{
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationSets::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler)
{
	m_ppAnimationSets[nAnimationSet]->SetAnimationCallbackHandler(pCallbackHandler);
}

void CAnimationSets::SetCallbackFuncKeys(int nAnimationSet, int nCallbackKeys)
{
	m_ppAnimationSets[nAnimationSet]->m_nCallbackFuncKeys = nCallbackKeys;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackFuncKeys = new CALLBACKFUNCKEY[nCallbackKeys];
}

void CAnimationSets::SetCallbackFuncKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void *pData1, int pData2, int pData3)
{
	m_ppAnimationSets[nAnimationSet]->m_pCallbackFuncKeys[nKeyIndex].m_fTime = fKeyTime;  //0번째 인덱스에 keytime 경과시 pdata3실행
	m_ppAnimationSets[nAnimationSet]->m_pCallbackFuncKeys[nKeyIndex].m_pAnimationController = pData1;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackFuncKeys[nKeyIndex].m_pCallbackPrevData = pData2;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackFuncKeys[nKeyIndex].m_pCallbackData = pData3;
}

void CAnimationSets::SetAnimationCallbackFuncHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler)
{
	m_ppAnimationSets[nAnimationSet]->SetAnimationCallbackFuncHandler(pCallbackHandler);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationController::CAnimationController(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nAnimationTracks, CLoadedModelInfo *pModel)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new CAnimationTrack[nAnimationTracks];

	m_pAnimationSets = pModel->m_pAnimationSets;
	m_pAnimationSets->AddRef();

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;
	m_ppSkinnedMeshes = new CSkinnedMesh*[m_nSkinnedMeshes];
	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource*[m_nSkinnedMeshes];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4*[m_nSkinnedMeshes];

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void **)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}
}

CAnimationController::~CAnimationController()
{
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;

	if (m_pAnimationSets) m_pAnimationSets->Release();

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Release();
	}
	if (m_ppd3dcbSkinningBoneTransforms) delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;

	if (m_ppSkinnedMeshes)
		delete[] m_ppSkinnedMeshes;
}

void CAnimationController::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKeys(nAnimationSet, nCallbackKeys);
}

void CAnimationController::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void *pData)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKey(nAnimationSet, nKeyIndex, fKeyTime, pData);
}

void CAnimationController::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler)
{
	if (m_pAnimationSets) m_pAnimationSets->SetAnimationCallbackHandler(nAnimationSet, pCallbackHandler);
}

void CAnimationController::SetCallbackFuncKeys(int nAnimationSet, int nCallbackKeys)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackFuncKeys(nAnimationSet, nCallbackKeys);
}

void CAnimationController::SetCallbackFuncKey(int nAnimationSet, int nKeyIndex, float fKeyTime, int pData2, int pData3)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackFuncKey(nAnimationSet, nKeyIndex, fKeyTime, this, pData2, pData3);
	// n_keyIndex 애니메이션의 몇번째 예약 애니메이션인지.   
	//KeyTime 발동 시간, pData2: 애니메이션이 교체되면서 false될 키프레임 , pData3: 애니메이션이 교체되면서 true될 키프레임
}

void CAnimationController::SetAnimationCallbackFuncHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler)
{
	if (m_pAnimationSets) m_pAnimationSets->SetAnimationCallbackFuncHandler(nAnimationSet, pCallbackHandler);
}

void CAnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetAnimationSet(nAnimationSet);
}

void CAnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetEnable(bEnable);
}

void CAnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetPosition(fPosition);
}

void CAnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetSpeed(fSpeed);
}

void CAnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetWeight(fWeight);
}

void CAnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i]->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[i];
		m_ppSkinnedMeshes[i]->m_pcbxmf4x4MappedSkinningBoneTransforms = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];
	}
}
void CAnimationController::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
		m_ppSkinnedMeshes[i]->ReleaseUploadBuffers();
}
void CAnimationController::AdvanceTime(float fTimeElapsed, CGameObject *pRootGameObject)
{
	m_fTime += fTimeElapsed;
	if (m_pAnimationTracks)
	{
		//for (int i = 0; i < m_nAnimationTracks; i++) 

		for (int k = 0; k < m_nAnimationTracks; k++)
		{

			if (m_pAnimationTracks[k].m_bEnable)
			{
				m_pAnimationTracks[k].m_fPosition += (fTimeElapsed * m_pAnimationTracks[k].m_fSpeed);
				CAnimationSet *pAnimationSet = m_pAnimationSets->m_ppAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
				pAnimationSet->Animate(m_pAnimationTracks[k].m_fPosition, m_pAnimationTracks[k].m_fWeight);
			}
		}

		//pRootGameObject->UpdateTransform(NULL);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CLoadedModelInfo::~CLoadedModelInfo()
{
	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void CLoadedModelInfo::PrepareSkinning()
{
	int nSkinnedMesh = 0;
	m_ppSkinnedMeshes = new CSkinnedMesh*[m_nSkinnedMeshes];
	m_pModelRootObject->FindAndSetSkinnedMesh(m_ppSkinnedMeshes, &nSkinnedMesh);

	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i]->PrepareSkinning(m_pModelRootObject);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject()
{
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::CGameObject(int nMaterials) : CGameObject()
{
	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new CMaterial*[m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	if (m_pMesh)
	{
		m_pMesh->ReleaseShaderVariables();
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				m_ppMaterials[i]->Release();
				m_ppMaterials[i] = NULL;
			}
		}
		delete[] m_ppMaterials;
		m_ppMaterials = NULL;
	}

	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
		m_pd3dcbGameObjects = NULL;
	}

	if (m_pSkinnedAnimationController) delete m_pSkinnedAnimationController;
}

void CGameObject::AddRef()
{
	m_nReferences++;

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void CGameObject::Release()
{
	if (m_pChild) m_pChild->Release();
	if (m_pSibling) m_pSibling->Release();

	if (--m_nReferences <= 0) delete this;
}

void CGameObject::SetChild(CGameObject *pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader *pShader)
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = new CMaterial(0);
	m_ppMaterials[0]->SetShader(pShader);
}

void CGameObject::SetShader(int nMaterial, CShader *pShader)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->SetShader(pShader);
}

void CGameObject::SetMaterial(int nMaterial, CMaterial *pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

CSkinnedMesh *CGameObject::FindSkinnedMesh(char *pstrSkinnedMeshName)
{
	CSkinnedMesh *pSkinnedMesh = NULL;
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT))
	{
		pSkinnedMesh = (CSkinnedMesh *)m_pMesh;
		if (!strcmp(pSkinnedMesh->m_pstrMeshName, pstrSkinnedMeshName)) return(pSkinnedMesh);
	}

	if (m_pSibling) if (pSkinnedMesh = m_pSibling->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);
	if (m_pChild) if (pSkinnedMesh = m_pChild->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);

	return(NULL);
}

void CGameObject::FindAndSetSkinnedMesh(CSkinnedMesh **ppSkinnedMeshes, int *pnSkinnedMesh)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT)) ppSkinnedMeshes[(*pnSkinnedMesh)++] = (CSkinnedMesh *)m_pMesh;

	if (m_pSibling) m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
	if (m_pChild) m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
}

CGameObject *CGameObject::FindFrame(char *pstrFrameName)
{
	CGameObject *pFrameObject = NULL;
	if (!strcmp(m_pstrFrameName, pstrFrameName)) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}
void CGameObject::UpdateTransform_Instancing(unordered_map<string, CB_OBJECT_INFO*>& instancedTransformBuffer, const int& idx, XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	XMStoreFloat4x4(&(instancedTransformBuffer[m_pstrFrameName][idx].m_xmf4x4World),
		XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	if (m_pSibling) m_pSibling->UpdateTransform_Instancing(instancedTransformBuffer, idx, pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform_Instancing(instancedTransformBuffer, idx, &m_xmf4x4World);
}

void CGameObject::UpdateTransform_SkinedInstancing(unordered_map<string, CB_SKINEOBJECT_INFO*>& instancedTransformBuffer, const int& idx)
{
	if (m_pMesh)
	{
		CSkinnedMesh* mesh = (CSkinnedMesh*)m_pMesh;
		for (int j = 0; j < mesh->m_nSkinningBones; j++)
		{
			XMStoreFloat4x4(&(instancedTransformBuffer[m_pstrFrameName][idx].m_xmf4x4Worlds[j]),
				XMMatrixTranspose(XMLoadFloat4x4(&mesh->m_ppSkinningBoneFrameCaches[j]->m_xmf4x4World)));
		}
	}
	if (m_pSibling) m_pSibling->UpdateTransform_SkinedInstancing(instancedTransformBuffer, idx);
	if (m_pChild) m_pChild->UpdateTransform_SkinedInstancing(instancedTransformBuffer, idx);
}

void CGameObject::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackAnimationSet(nAnimationTrack, nAnimationSet);
}

void CGameObject::SetTrackAnimationPosition(int nAnimationTrack, float fPosition)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackPosition(nAnimationTrack, fPosition);
}

void CGameObject::FixedUpdate(float fTimeElapsed)
{

	float drag = 0.5* cPlayer* AIR *2.2;
	float rR = drag * 30;
	XMFLOAT3 xmf3Look = GetLook();
	XMFLOAT3 xmf3Fdrag = Vector3::ScalarProduct(m_xmf3Velocity, -drag * Vector3::Length(m_xmf3Velocity), false);
	XMFLOAT3 xmf3Frr = Vector3::ScalarProduct(m_xmf3Velocity, -rR, false);
	XMFLOAT3 xmf3Ftraction = Vector3::ScalarProduct(xmf3Look, m_fForce, false);
	xmf3Ftraction = Vector3::Add(xmf3Ftraction, xmf3Frr);
	xmf3Ftraction = Vector3::Add(xmf3Ftraction, xmf3Fdrag);  //총합
	xmf3Ftraction = Vector3::Add(xmf3Ftraction, m_xmf3Forces);
	m_xmf3AcceleratingForce = Vector3::DivProduct(xmf3Ftraction, m_fMass, false);
	if (m_xmf3AcceleratingForce.z < 0)
		drag = 1;
	Move(Vector3::ScalarProduct(m_xmf3AcceleratingForce, fTimeElapsed, false), true);
	//float fMaxVelocityY = m_fMaxVelocityY;
	//fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	//if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, 10 * fTimeElapsed, false);
	Move(xmf3Velocity, false);
	
	m_fForce = 0;
	m_xmf3Forces = XMFLOAT3(0, 0, 0);
}
void CGameObject::Move(const XMFLOAT3& xmf3Shift, bool bVelocity)
{
	if (bVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf4x4ToParent._41 += xmf3Shift.x;
		m_xmf4x4ToParent._42 += xmf3Shift.y;
		m_xmf4x4ToParent._43 += xmf3Shift.z;
	}
}
void CGameObject::Animate(float fTimeElapsed)
{
	OnPrepareRender();

	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);


	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);

	UpdateTransform(NULL);
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh)
	{
		if (IsVisible(pCamera))
		{
			if (isSkined)
				UpdateShaderVariable(pd3dCommandList);
			else
				UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
			if (m_nMaterials > 0)
			{
				for (int i = 0; i < m_nMaterials; i++)
				{
					if (m_ppMaterials[i])
					{
						if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
						m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
					}

					m_pMesh->Render(pd3dCommandList, i);
				}
			}
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera,
	UINT nInstances)
{
	//인스턴싱용 랜더함수
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	OnPrepareRender();
	if (m_pMesh)
	{
		if (isSkined)
		{
			if (m_pcbMappedSkinedGameObjects)
				pd3dCommandList->SetGraphicsRootShaderResourceView(3,
					m_pd3dcbSkinedGameObjects->GetGPUVirtualAddress());
		}
		else
		{
			if (m_pcbMappedGameObjects)
				pd3dCommandList->SetGraphicsRootShaderResourceView(3,
					m_pd3dcbGameObjects->GetGPUVirtualAddress());
		}
		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; ++i)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader)
					{
						m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
					}
					if (m_ppMaterials[i]->m_ppTextures)
					{
						for (int j = 0; j < m_ppMaterials[i]->m_nTextures; ++j)
						{
							m_ppMaterials[i]->m_ppTextures[j]->UpdateShaderVariables(pd3dCommandList);
						}

					}
					m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
				}

				m_pMesh->Render(pd3dCommandList, i, nInstances);
			}
		}
	}
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera, nInstances);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera, nInstances);

}
//-------
#ifdef _WITH_BOUND_BOX
void CGameObject::BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_pMesh)
	{
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		
		m_pMesh->BbxRender(pd3dCommandList);
	}

	if (m_pSibling) m_pSibling->BbxRender(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->BbxRender(pd3dCommandList, pCamera);
}

void CGameObject::BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera,
	UINT nInstances)
{

	pd3dCommandList->SetGraphicsRootShaderResourceView(3,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());

	if (m_pMesh) m_pMesh->BbxRender(pd3dCommandList, nInstances);
	
	if (m_pSibling) m_pSibling->BbxRender(pd3dCommandList, pCamera, nInstances);
	if (m_pChild) m_pChild->BbxRender(pd3dCommandList, pCamera, nInstances);
}
#endif


//----
void CGameObject::ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh)
	{
		if (IsVisible(pCamera))
		{
			if (isSkined)
				UpdateShaderVariable(pd3dCommandList);
			else
				UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

			if (m_nMaterials > 0)
			{
				for (int i = 0; i < m_nMaterials; i++)
				{
					//if (m_ppMaterials[i])
					//{
					//	if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
					//	m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
					//}

					m_pMesh->Render(pd3dCommandList, i);
				}
			}
		}
	}

	if (m_pSibling) m_pSibling->ShadowRender(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->ShadowRender(pd3dCommandList, pCamera);
}

void CGameObject::ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera,
	UINT nInstances)
{
	//인스턴싱용 랜더함수
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	OnPrepareRender();
	if (m_pMesh)
	{
		if (isSkined)
			pd3dCommandList->SetGraphicsRootShaderResourceView(3,
				m_pd3dcbSkinedGameObjects->GetGPUVirtualAddress());
		else
			pd3dCommandList->SetGraphicsRootShaderResourceView(3,
				m_pd3dcbGameObjects->GetGPUVirtualAddress());

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; ++i)
			{
				//if (m_ppMaterials[i])
				//{
				//	if (m_ppMaterials[i]->m_pShader)
				//	{
				//		m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
				//	}
				//	if (m_ppMaterials[i]->m_ppTextures)
				//	{
				//		for (int j = 0; j < m_ppMaterials[i]->m_nTextures; ++j)
				//		{
				//			m_ppMaterials[i]->m_ppTextures[j]->UpdateShaderVariables(pd3dCommandList);
				//		}
				//	
				//	}
				//	m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
				//}

				m_pMesh->Render(pd3dCommandList, i, nInstances);
			}
		}
	}
	if (m_pSibling) m_pSibling->ShadowRender(pd3dCommandList, pCamera, nInstances);
	if (m_pChild) m_pChild->ShadowRender(pd3dCommandList, pCamera, nInstances);

}

void CGameObject::CreateShaderVariables(shared_ptr<CreateManager> pCreateManager)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(2, 16, &xmf4x4World, 0);
}
void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList)
{
	CSkinnedMesh* mesh = (CSkinnedMesh*)m_pMesh;
	if (mesh->m_pd3dcbSkinningBoneTransforms)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneTransformsGpuVirtualAddress = mesh->m_pd3dcbSkinningBoneTransforms->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(7, d3dcbBoneTransformsGpuVirtualAddress); //Skinned Bone Transforms

		for (int j = 0; j < mesh->m_nSkinningBones; j++)
		{
			XMStoreFloat4x4(&mesh->m_pcbxmf4x4MappedSkinningBoneTransforms[j], XMMatrixTranspose(XMLoadFloat4x4(&mesh->m_ppSkinningBoneFrameCaches[j]->m_xmf4x4World)));
		}
	}
}
void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterial *pMaterial)
{

}

void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
		m_pd3dcbGameObjects = NULL;
	}
	if (m_pd3dcbSkinedGameObjects)
	{
		m_pd3dcbSkinedGameObjects->Unmap(0, NULL);
		m_pd3dcbSkinedGameObjects->Release();
		m_pd3dcbSkinedGameObjects = NULL;
	}
	if (m_pSibling) m_pSibling->ReleaseShaderVariables();
	if (m_pChild) m_pChild->ReleaseShaderVariables();
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
	if (m_pSkinnedAnimationController)
		m_pSkinnedAnimationController->ReleaseUploadBuffers();
	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i])
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParent._41 = x;
	m_xmf4x4ToParent._42 = y;
	m_xmf4x4ToParent._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4 *pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

//#define _WITH_DEBUG_FRAME_HIERARCHY

int ReadIntegerFromFile(FILE *pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE *pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

int ReadStringFromFile(FILE *pInFile, char *pstrToken)
{
	int nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

void CGameObject::LoadMaterialsFromFile(shared_ptr<CreateManager> pCreateManager, CGameObject *pParent, FILE *pInFile, CShader *pShader)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;
	int nTexture = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	m_ppMaterials = new CMaterial*[m_nMaterials];
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	CMaterial *pMaterial = NULL;

	for (; ; )
	{
		ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTexture, sizeof(int), 1, pInFile);

			pMaterial = new CMaterial(nTexture); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal						
			CTexture *pTexture;
			if (nTexture > 0)
			{
				CShader* shader = new CShader();
				shader->CreateCbvSrvDescriptorHeaps(pCreateManager, 0, nTexture);
				//ReadStringFromFile(pInFile, pstrToken);
				for (int j = 0; j < nTexture; ++j)
				{
					ReadStringFromFile(pInFile, pstrToken);
					CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
					pTexture->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), ConvertCHARtoWCHAR(pstrToken), 0);
					shader->CreateShaderResourceViews(pCreateManager, pTexture, 8 + j, true);
					pMaterial->SetTexture(pTexture, j);
				}
				pMaterial->SetShader(shader);
			}

			ReadStringFromFile(pInFile, pstrToken);

			if (!strcmp(pstrToken, "<Phong>:"))
			{
				(UINT)::fread(&pMaterial->m_xmf4AmbientColor, sizeof(float), 3, pInFile);
				(UINT)::fread(&pMaterial->m_xmf4DiffuseColor, sizeof(float), 3, pInFile);
				(UINT)::fread(&pMaterial->m_xmf4SpecularColor, sizeof(float), 3, pInFile);
				(UINT)::fread(&pMaterial->m_xmf4EmissiveColor, sizeof(float), 3, pInFile);
			}
			else if (!strcmp(pstrToken, "<Lambert>:"))
			{
				(UINT)::fread(&pMaterial->m_xmf4AmbientColor, sizeof(float), 3, pInFile);
				(UINT)::fread(&pMaterial->m_xmf4DiffuseColor, sizeof(float), 3, pInFile);
				//(UINT)::fread(&pMaterial->m_xmf4SpecularColor, sizeof(float), 3, pInFile);
				(UINT)::fread(&pMaterial->m_xmf4EmissiveColor, sizeof(float), 3, pInFile);
			}
			else if (!strcmp(pstrToken, "<Unknown>"))
				continue;
			pMaterial->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
			SetMaterial(nMaterial, pMaterial);

		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

CGameObject *CGameObject::LoadFrameHierarchyFromFile(shared_ptr<CreateManager> pCreateManager, CGameObject *pParent, FILE *pInFile, CShader *pShader, int *pnSkinnedMeshes)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = ::ReadIntegerFromFile(pInFile);

	CGameObject *pGameObject = new CGameObject();
	::ReadStringFromFile(pInFile, pGameObject->m_pstrFrameName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT4X4 mat;
			nReads = (UINT)::fread(&mat, sizeof(XMFLOAT4X4), 1, pInFile);
			pGameObject->m_xmf4x4ToParent = mat;
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CMesh *pMesh = new CMesh(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
			pMesh->LoadMeshFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), pInFile);
			pGameObject->SetMesh(pMesh);

		}
		else if (!strcmp(pstrToken, "<SkinDeformations>:"))
		{
			pGameObject->isSkined = true;
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			CSkinnedMesh *pSkinnedMesh = new CSkinnedMesh(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
			pSkinnedMesh->LoadSkinDeformationsFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), pInFile);
			pSkinnedMesh->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

			::ReadStringFromFile(pInFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) pSkinnedMesh->LoadMeshFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), pInFile);

			pGameObject->SetMesh(pSkinnedMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pCreateManager, pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Frame>:"))
					{
						CGameObject *pChild = CGameObject::LoadFrameHierarchyFromFile(pCreateManager,  pGameObject, pInFile, pShader, pnSkinnedMeshes);
						if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
						TCHAR pstrDebug[256] = { 0 };
						_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
						OutputDebugString(pstrDebug);
#endif
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

void CGameObject::PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

void CGameObject::LoadAnimationFromFile(FILE *pInFile, CLoadedModelInfo *pLoadedModel)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nAnimationSets = 0;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nAnimationSets = ::ReadIntegerFromFile(pInFile);
			pLoadedModel->m_pAnimationSets = new CAnimationSets(nAnimationSets);
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			int nAnimationSet = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pstrToken); //Animation Set Name

			float fStartTime = ::ReadFloatFromFile(pInFile);
			float fEndTime = ::ReadFloatFromFile(pInFile);

			pLoadedModel->m_pAnimationSets->m_ppAnimationSets[nAnimationSet] = new CAnimationSet(fStartTime, fEndTime, pstrToken);
			CAnimationSet *pAnimationSet = pLoadedModel->m_pAnimationSets->m_ppAnimationSets[nAnimationSet];

			::ReadStringFromFile(pInFile, pstrToken);
			if (!strcmp(pstrToken, "<AnimationLayers>:"))
			{
				pAnimationSet->m_nAnimationLayers = ::ReadIntegerFromFile(pInFile);
				pAnimationSet->m_pAnimationLayers = new CAnimationLayer[pAnimationSet->m_nAnimationLayers];

				for (int i = 0; i < pAnimationSet->m_nAnimationLayers; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<AnimationLayer>:"))
					{
						int nAnimationLayer = ::ReadIntegerFromFile(pInFile);
						CAnimationLayer *pAnimationLayer = &pAnimationSet->m_pAnimationLayers[nAnimationLayer];

						pAnimationLayer->m_nAnimatedBoneFrames = ::ReadIntegerFromFile(pInFile);

						pAnimationLayer->m_ppAnimatedBoneFrameCaches = new CGameObject *[pAnimationLayer->m_nAnimatedBoneFrames];
						pAnimationLayer->m_ppAnimationCurves = new CAnimationCurve *[pAnimationLayer->m_nAnimatedBoneFrames][9];

						pAnimationLayer->m_fWeight = ::ReadFloatFromFile(pInFile);

						for (int j = 0; j < pAnimationLayer->m_nAnimatedBoneFrames; j++)
						{
							::ReadStringFromFile(pInFile, pstrToken);
							if (!strcmp(pstrToken, "<AnimationCurve>:"))
							{
								int nCurveNode = ::ReadIntegerFromFile(pInFile); //j

								for (int k = 0; k < 9; k++) pAnimationLayer->m_ppAnimationCurves[j][k] = NULL;

								::ReadStringFromFile(pInFile, pstrToken);
								pAnimationLayer->m_ppAnimatedBoneFrameCaches[j] = pLoadedModel->m_pModelRootObject->FindFrame(pstrToken);

								for (; ; )
								{
									::ReadStringFromFile(pInFile, pstrToken);
									if (!strcmp(pstrToken, "<TX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 0, pInFile);
									else if (!strcmp(pstrToken, "<TY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 1, pInFile);
									else if (!strcmp(pstrToken, "<TZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 2, pInFile);
									else if (!strcmp(pstrToken, "<RX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 3, pInFile);
									else if (!strcmp(pstrToken, "<RY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 4, pInFile);
									else if (!strcmp(pstrToken, "<RZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 5, pInFile);
									else if (!strcmp(pstrToken, "<SX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 6, pInFile);
									else if (!strcmp(pstrToken, "<SY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 7, pInFile);
									else if (!strcmp(pstrToken, "<SZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 8, pInFile);
									else if (!strcmp(pstrToken, "</AnimationCurve>"))
									{
										break;
									}
								}
							}
						}
						::ReadStringFromFile(pInFile, pstrToken); //</AnimationLayer>
					}
				}
				::ReadStringFromFile(pInFile, pstrToken); //</AnimationLayers>
			}
			::ReadStringFromFile(pInFile, pstrToken); //</AnimationSet>
		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			break;
		}
	}
}

CLoadedModelInfo *CGameObject::LoadGeometryAndAnimationFromFile(shared_ptr<CreateManager> pCreateManager,  const char *pstrFileName, CShader *pShader)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CLoadedModelInfo *pLoadedModel = new CLoadedModelInfo();
	pLoadedModel->m_pModelRootObject = new CGameObject(0);
	strcpy_s(pLoadedModel->m_pModelRootObject->m_pstrFrameName, "RootNode");

	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		if (::ReadStringFromFile(pInFile, pstrToken))
		{
			if (!strcmp(pstrToken, "<Hierarchy>"))
			{
				for (; ; )
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Frame>:"))
					{
						CGameObject *pChild = CGameObject::LoadFrameHierarchyFromFile(pCreateManager,  NULL, pInFile, pShader, &pLoadedModel->m_nSkinnedMeshes);
						if (pChild) pLoadedModel->m_pModelRootObject->SetChild(pChild);
					}
					else if (!strcmp(pstrToken, "</Hierarchy>"))
					{
						break;
					}
				}
			}
			else if (!strcmp(pstrToken, "<Animation>"))
			{
				CGameObject::LoadAnimationFromFile(pInFile, pLoadedModel);
				pLoadedModel->PrepareSkinning();
			}
			else if (!strcmp(pstrToken, "</Animation>"))
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pLoadedModel->m_pModelRootObject, NULL);
#endif

	::fclose(pInFile);

	return(pLoadedModel);
}


void CGameObject::CreateInstanceBuffer(shared_ptr<CreateManager> pCreateManager,
	UINT nInstances, unordered_map<string, CB_OBJECT_INFO*>& instancedTransformBuffer)
{
	if (nInstances > 0)
	{
		m_pd3dcbGameObjects = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
			sizeof(CB_OBJECT_INFO) * nInstances, D3D12_HEAP_TYPE_UPLOAD,
			D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
		//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다. 
		m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
		instancedTransformBuffer[m_pstrFrameName] = m_pcbMappedGameObjects;

		if (m_pSibling) m_pSibling->CreateInstanceBuffer(pCreateManager, nInstances, instancedTransformBuffer);
		if (m_pChild) m_pChild->CreateInstanceBuffer(pCreateManager, nInstances, instancedTransformBuffer);
	}
}

void CGameObject::CreateSkinedInstanceBuffer(shared_ptr<CreateManager> pCreateManager,
	UINT nInstances, unordered_map<string, CB_SKINEOBJECT_INFO*>& instancedTransformBuffer)
{
	if (nInstances > 0)
	{
		m_pd3dcbSkinedGameObjects = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
			sizeof(CB_SKINEOBJECT_INFO) * nInstances, D3D12_HEAP_TYPE_UPLOAD,
			D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
		//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다. 
		m_pd3dcbSkinedGameObjects->Map(0, NULL, (void **)&m_pcbMappedSkinedGameObjects);
		instancedTransformBuffer[m_pstrFrameName] = m_pcbMappedSkinedGameObjects;

		if (m_pSibling) m_pSibling->CreateSkinedInstanceBuffer(pCreateManager, nInstances, instancedTransformBuffer);
		if (m_pChild) m_pChild->CreateSkinedInstanceBuffer(pCreateManager, nInstances, instancedTransformBuffer);
	}
}

bool CGameObject::IsVisible(CCamera *pCamera)
{
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();
	//모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다. 
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
	return(bIsVisible);
}

bool CGameObject::IsCollide(CGameObject* ob)
{
	//해당 오브젝트와 ob 충돌 이안에서 ob와의 충돌을 전부 끝내야함
	if (!ob->isEnable)
		return false;

	//if (Vector3::Length(GetPosition(), ob->GetPosition()) > COLLISION_LENGTH) 
	//	return false; //  COLLISION_LENGTH보다 거리가 멀면 충돌처리 x

	if (IsCollide(this, ob))
		return true;

	if (m_pSibling) if (m_pSibling->IsCollide(ob)) return true;
	if (m_pChild) if (m_pChild->IsCollide(ob)) return true;

	return false;
}
bool CGameObject::IsCollide(CGameObject* self, CGameObject* target)
{
	//해당 오브젝트와 ob 충돌 이안에서 ob와의 충돌을 전부 끝내야함
	if (self->m_pMesh != NULL && target->m_pMesh != NULL)
	{
		BoundingOrientedBox boxA = self->m_pMesh->GetBoundingBox();
		BoundingOrientedBox boxB = target->m_pMesh->GetBoundingBox();
		boxA.Transform(boxA, XMLoadFloat4x4(&self->m_xmf4x4World));
		boxB.Transform(boxB, XMLoadFloat4x4(&target->m_xmf4x4World));
		if (boxA.Intersects(boxB))
			return true;
	}
	if (target->m_pSibling) if (IsCollide(self, target->m_pSibling)) return true;
	if (target->m_pChild) if (IsCollide(self, target->m_pChild)) return true;

	return false;
}