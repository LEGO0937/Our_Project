#pragma once
#include "../../Common/stdafx.h"
#include "../../Objects/BaseObject.h"

#define ANIMATION_TYPE_ONCE			0
#define ANIMATION_TYPE_LOOP			1
#define ANIMATION_TYPE_PINGPONG		2

#define ANIMATION_CALLBACK_EPSILON	0.025f

struct CALLBACKKEY
{
	float  							m_fTime = 0.0f;
	void  							*m_pCallbackData = NULL;
};
struct CALLBACKFUNCKEY
{
	float  							m_fTime = 0.0f;
	int  							m_pCallbackPrevData = NULL;
	int  							m_pCallbackData = NULL;
	void  							*m_pAnimationController = NULL;
};
struct CALLBACKFUNCData
{
	int  							m_pCallbackPrevData = NULL;
	int  							m_pCallbackData = NULL;
	void  							*m_pAnimationController = NULL;
};
#define _WITH_ANIMATION_INTERPOLATION

class CAnimationCallbackHandler
{
public:
	CAnimationCallbackHandler() { }
	~CAnimationCallbackHandler() { }

public:
	virtual void HandleCallback(void *pCallbackData) { }
	virtual void HandleCallback(void *pAnimationController, int nSet) { }
};
class CSoundCallbackHandler : public CAnimationCallbackHandler
{
public:
	CSoundCallbackHandler() { }
	~CSoundCallbackHandler() { }

public:
	virtual void HandleCallback(void *pCallbackData);
};

class CFuncCallbackHandler : public CAnimationCallbackHandler
{
public:
	CFuncCallbackHandler() { }
	~CFuncCallbackHandler() { }

public:
	void HandleCallback(void *pAnimationController, int nSet);
};

class CAnimationCurve
{
public:
	CAnimationCurve(int nKeys);
	~CAnimationCurve();

public:
	int								m_nKeys = 0;

	float							*m_pfKeyTimes = NULL;
	float							*m_pfKeyValues = NULL;

public:
	float GetValueByLinearInterpolation(float fPosition);
};

class CAnimationLayer
{
public:
	CAnimationLayer();
	~CAnimationLayer();

public:
	float							m_fWeight = 1.0f;

	int								m_nAnimatedBoneFrames = 0;
	CGameObject						**m_ppAnimatedBoneFrameCaches = NULL; //[m_nAnimatedBoneFrames]

	CAnimationCurve					*(*m_ppAnimationCurves)[9] = NULL;

public:
	void LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE *pInFile);
	XMFLOAT4X4 GetSRT(int nBoneFrame, float fPosition);
};

class CAnimationSet
{
public:
	CAnimationSet(float fStartTime, float fEndTime, char *pstrName);
	~CAnimationSet();

public:
	char							m_pstrAnimationSetName[64];

	int								m_nAnimationLayers = 0;
	CAnimationLayer					*m_pAnimationLayers = NULL;

	float							m_fStartTime = 0.0f;
	float							m_fEndTime = 0.0f;
	float							m_fLength = 0.0f;

	float 							m_fPosition = 0.0f;
	int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong

	int 							m_nCallbackKeys = 0;
	int 							m_nCallbackFuncKeys = 0;

	CALLBACKKEY 					*m_pCallbackKeys = NULL;
	CALLBACKFUNCKEY 				*m_pCallbackFuncKeys = NULL;

	CAnimationCallbackHandler 		*m_pAnimationCallbackHandler = NULL;
	CAnimationCallbackHandler 		*m_pAnimationCallbackFuncHandler = NULL;

public:
	void SetPosition(float fTrackPosition);

	void Animate(float fTrackPosition, float fTrackWeight);

	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(CAnimationCallbackHandler *pCallbackHandler);

	void SetCallbackFuncKeys(int nCallbackKeys);
	void SetCallbackFuncKey(int nKeyIndex, float fTime, void *pData1, int pDate2, int pData3);
	void SetAnimationCallbackFuncHandler(CAnimationCallbackHandler *pCallbackHandler);

	void *GetCallbackData();
	void *GetCallbackFuncData(CALLBACKFUNCData& data);
};

class CAnimationSets
{
private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CAnimationSets(int nAnimationSets);
	~CAnimationSets();

public:
	int								m_nAnimationSets = 0;
	CAnimationSet					**m_ppAnimationSets = NULL;

public:
	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler);

	void SetCallbackFuncKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackFuncKey(int nAnimationSet, int nKeyIndex, float fTime, void *pData1, int pData2, int pData3);
	void SetAnimationCallbackFuncHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler);
};

class CAnimationTrack
{
public:
	CAnimationTrack() { }
	~CAnimationTrack() { }

public:
	BOOL 							m_bEnable = false;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	float 							m_fWeight = 1.0f;

	int 							m_nAnimationSet = 0;

public:
	void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }

	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetWeight(float fWeight) { m_fWeight = fWeight; }
	void SetPosition(float fPosition) { m_fPosition = fPosition; }
};



class CAnimationController
{
public:
	CAnimationController(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nAnimationTracks, CLoadedModelInfo *pModel);
	~CAnimationController();

public:
	float 							m_fTime = 0.0f;

	int 							m_nAnimationTracks = 0;
	CAnimationTrack 				*m_pAnimationTracks = NULL;

	CAnimationSets					*m_pAnimationSets = NULL;

	int 							m_nSkinnedMeshes = 0;
	CSkinnedMesh					**m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

	ID3D12Resource					**m_ppd3dcbSkinningBoneTransforms = NULL; //[SkinnedMeshes]
	XMFLOAT4X4						**m_ppcbxmf4x4MappedSkinningBoneTransforms = NULL;

	int								m_CurrentTrack = 0;
public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackEnable(int nAnimationTrack, bool bEnable);
	void SetTrackPosition(int nAnimationTrack, float fPosition);
	void SetTrackSpeed(int nAnimationTrack, float fSpeed);
	void SetTrackWeight(int nAnimationTrack, float fWeight);

	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void *pData);
	void SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler);

	void SetCallbackFuncKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackFuncKey(int nAnimationSet, int nKeyIndex, float fTime, int pData2, int pData3);
	void SetAnimationCallbackFuncHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler);

	void ReleaseUploadBuffers();

	void AdvanceTime(float fElapsedTime, CGameObject *pRootGameObject);
};