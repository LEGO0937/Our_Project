#pragma once
#include "../Common/Timer/Timer.h"
#include "BaseObject.h"
#include "../CShaders/FontShader/FontShader.h"


class CPlayer;
class CCamera;
class CreateManager;
class NetWorkManager;
class SoundManager;

struct LIGHT
{
	XMFLOAT4 m_xmf4Ambient = XMFLOAT4(0.0f,0.0f,0.0f,1.0f);
	XMFLOAT4 m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float m_fFalloff = 0.0f;

	XMFLOAT3 m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float m_fTheta = 0.0f; //cos(m_fTheta)

	XMFLOAT3 m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.001f);;
	float m_fPhi = 0.0f; //cos(m_fPhi)

	int m_nType = 1;
	float m_fRange = 0.0f;
	float padding = 0.0f;
	int m_bEnable = 0;
};

struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	float fogstart = 0.1f;
	float fogrange = 0.0f;
	float padding0 = 0.0f;
	float padding1 = 0.0f;
};

struct CB_GAME_INFO
{
	XMFLOAT4X4 m_xmf4x4ShadowView;
	XMFLOAT4X4 m_xmf4x4ShadowProjection;
	XMFLOAT4X4 m_xmf4x4InvShadowViewProjection;
	XMFLOAT3 m_xmf3ShadowCameraPosition;
};

class BaseScene
{
public:
	POINT m_ptOldCursorPos;

	BaseScene();
	~BaseScene();

	//씬에서 마우스와 키보드 메시지를 처리한다. 
	virtual void ProcessInput(HWND hWnd, float deltaTime) = 0;
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime) = 0;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime) = 0;

	virtual void SetViewportsAndScissorRects();

	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager) = 0;
	virtual void BuildLights() {};

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(CreateManager* pCreateManager) {}
	virtual void UpdateShaderVariables() {}
	virtual void ReleaseShaderVariables() {}

	virtual SceneType Update(CreateManager* pCreateManager, float fTimeElapsed) = 0;
	virtual void FixedUpdate(CreateManager* pCreateManager, float fTimeElapsed) {};
	virtual void AnimateObjects(float fTimeElapsed) {}

	virtual void Render();
	virtual void RenderShadow();
	virtual void RenderVelocity() {}
	virtual void RenderPostProcess(ComPtr<ID3D12Resource> curBuffer, ComPtr<ID3D12Resource> velocityMap) {};

	void SetGraphicsRootSignature(ID3D12RootSignature *pd3dRootSignature);
	void SetPipelineStates(int nPipelineStates, ID3D12PipelineState** ppd3dPipelineStates);
	void SetFontShader(FontShader* shader) { fontShader = shader; }

	virtual void ResetShadowBuffer(CreateManager* pCreateManager) {};

	CPlayer* m_pPlayer = NULL;
	CCamera* m_pCamera = NULL;
	CCamera* m_pShadowCamera = NULL;

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

	virtual void ReSize(shared_ptr<CreateManager> pCreateManager);

	virtual string GetId() { return m_sPlayerId; }
	virtual void SetId(const string& str) { m_sPlayerId = str; }

	//--------------------EventHandler---
	virtual void ProcessEvent(const MessageStruct& msg) {}
	//---------NetWork
	virtual void ProcessPacket(char* packet,float fTimeElapsed) {}
protected:
	string m_sPlayerId;
	shared_ptr<CreateManager> m_pCreateManager = NULL;
	shared_ptr<SoundManager> m_pSoundManager = NULL;
	SceneType sceneType;

	ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	int m_nPipelineStates = 0;
	ID3D12PipelineState **m_ppd3dPipelineStates = NULL;

	LIGHTS *m_pLights = NULL;     

	ID3D12Resource *m_pd3dcbLights = NULL;
	LIGHTS *m_pcbMappedLights = NULL;

	FontShader* fontShader = NULL;
	vector<GameText> gameTexts;

	int m_nWndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_nWndClientHeight{ FRAME_BUFFER_HEIGHT };
};
