#include "Animation.h"

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
	
}

void CAnimationSet::Animate(float fTrackPosition, float fTrackWeight)
{
	SetPosition(fTrackPosition);

	for (int i = 0; i < m_nAnimationLayers; i++)
	{
		for (int j = 0; j < m_pAnimationLayers[i].m_nAnimatedBoneFrames; j++)
		{
			//모션블러 연습중
			m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4PrevWorld = m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4World;
			
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