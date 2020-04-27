#pragma once
#include "../Common/Timer/Timer.h"
#include "../Objects/BaseObject.h"
#include "../CShaders/FontShader/FontShader.h"

//enum renderItem {
//	opacity=0,terrain, transparency
//};

class CPlayer;
class CCamera;
class CreateManager;
struct LIGHT
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT3 m_xmf3Position;
	float m_fFalloff;
	XMFLOAT3 m_xmf3Direction;
	float m_fTheta; //cos(m_fTheta)
	XMFLOAT3 m_xmf3Attenuation;
	float m_fPhi; //cos(m_fPhi)
	bool m_bEnable;
	int m_nType;
	float m_fRange;
	float padding;
};

struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient;
	float fogstart;
	float fogrange;
};


class BaseScene
{
public:
	POINT m_ptOldCursorPos;

	BaseScene();
	~BaseScene();

	//������ ���콺�� Ű���� �޽����� ó���Ѵ�. 
	virtual void ProcessInput(HWND hWnd, float deltaTime) = 0;
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime) = 0;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime) = 0;

	virtual void SetViewportsAndScissorRects();

	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager) = 0;
	virtual void BuildLights() = 0;

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(shared_ptr<CreateManager> pCreateManager) {}
	virtual void UpdateShaderVariables() {}
	virtual void ReleaseShaderVariables() {}

	virtual SceneType Update(float fTimeElapsed) = 0;
	virtual void FixedUpdate(float fTimeElapsed) {};
	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Render(float fTimeElapsed);
	virtual void RenderShadow();
	virtual void RenderPostProcess(ComPtr<ID3D12Resource> curBuffer) {};

	void SetGraphicsRootSignature(ID3D12RootSignature *pd3dRootSignature);
	void SetPipelineStates(int nPipelineStates, ID3D12PipelineState** ppd3dPipelineStates);
	void SetFontShader(shared_ptr<FontShader> shader) { fontShader = shader; }

	virtual void ResetShadowBuffer(shared_ptr<CreateManager> pCreateManager) {};

	CPlayer* m_pPlayer = NULL;
	CCamera* m_pCamera = NULL;
	string m_sPlayerId;

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

	void SetWindowSize(const int& width, const int& height) {
		m_nWndClientWidth = width; m_nWndClientHeight = height;
	}

	virtual string GetId() { return ""; }
	virtual string GetPassWord() { return ""; }
protected:
	SceneType sceneType;

	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList = NULL;

	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature = NULL;

	int m_nPipelineStates = 0;
	ID3D12PipelineState **m_ppd3dPipelineStates = NULL;

	LIGHTS *m_pLights = NULL;     

	ID3D12Resource *m_pd3dcbLights = NULL;
	LIGHTS *m_pcbMappedLights = NULL;

	shared_ptr<FontShader> fontShader = NULL;
	vector<GameText> gameTexts;

	int m_nWndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_nWndClientHeight{ FRAME_BUFFER_HEIGHT };
};
