#pragma once
#include "../Common/Timer/Timer.h"
#include "../Objects/BaseObject.h"
//enum renderItem {
//	opacity=0,terrain, transparency
//};

class CPlayer;
class CHeightMapTerrain;
class SkyBoxObject;
class CCamera;

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



	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList) = 0;
	virtual void BuildLights() = 0;

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();
	


	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();


	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Update(float fTimeElapsed) {};
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

	void SetGraphicsRootSignature(ID3D12RootSignature *pd3dRootSignature);
	void SetPipelineStates(int nPipelineStates, ID3D12PipelineState** ppd3dPipelineStates);

	CPlayer* m_pPlayer = NULL;
	CHeightMapTerrain* m_pTerrain = NULL;
	SkyBoxObject* m_pSkyBox = NULL;
	CCamera* m_pCamera = NULL;

	void setPlayer(CPlayer* player);
	void setCamera(CCamera* camera);
	

protected:
	//���� ���� ��ü���� �����̴�. ���� ��ü�� ���̴��� �����Ѵ�. 
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	ID3D12Device *m_pd3dDevice = NULL;
	ID3D12GraphicsCommandList *m_pd3dCommandList = NULL;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;

	int m_nPipelineStates = 0;
	ID3D12PipelineState **m_ppd3dPipelineStates = NULL;


	//CPlayer *m_pPlayer = NULL;   //�÷��̾� ��ü�� ���� �������̴�. 
	//CCamera *m_pCamera = NULL;


	LIGHTS *m_pLights = NULL;     //������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�.
	ID3D12Resource *m_pd3dcbLights = NULL;
	LIGHTS *m_pcbMappedLights = NULL;
};
