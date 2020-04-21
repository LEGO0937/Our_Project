#pragma once
#include "BaseScene.h"
//#include "../CShaders/BlurShader/BlurShader.h"

#define TerrainScaleX 25
#define TerrainScaleY 0.7
#define TerrainScaleZ 25

class CObInstancingShader;
class CSkinedObInstancingShader;
class CObjectsShader;
class CUiShader;
class BlurShader;

class CHeightMapTerrain;
class SkyBoxObject;
struct CB_GAME_INFO
{
	XMFLOAT4X4 m_xmf4x4ShadowView;
	XMFLOAT4X4 m_xmf4x4ShadowProjection;
	XMFLOAT4X4 m_xmf4x4InvShadowViewProjection;
	XMFLOAT3 m_xmf3ShadowCameraPosition;
};


class GameScene : public BaseScene
{
public:
	GameScene();
	~GameScene();

	void ProcessInput(HWND hWnd, float deltaTime);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);

	virtual void BuildLights();
	void BuildMinimapCamera(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);

	void BuildObjects(shared_ptr<CreateManager> pCreateManager);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(shared_ptr<CreateManager> pCreateManager);
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	void UpdateShadow();
	
	virtual void AnimateObjects(float fTimeElapsed);  //�ִϸ��̼� ������Ʈ
	virtual SceneType Update(float fTimeElapsed);  // �浹ó�� �� ����
	virtual void FixedUpdate(float fTimeElapsed);

	void Render();
	void RenderShadow();
	virtual void RenderPostProcess(ComPtr<ID3D12Resource> curBuffer);

	virtual void ResetShadowBuffer(shared_ptr<CreateManager> pCreateManager);

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

	CHeightMapTerrain* GetTerrain() { return m_pTerrain; }

private:
	ID3D12Resource *m_pd3dcbShadow = NULL;
	CB_GAME_INFO* m_pcbMappedShadow = NULL;

	SkyBoxObject* m_pSkyBox = NULL;
	CHeightMapTerrain* m_pTerrain = NULL;

	list<CObjectsShader*> UpdatedShaders;  //���������� �Ǵ� ���̴��� ����

	vector<CObInstancingShader*> instacingBillBoardShaders;
	vector<CObInstancingShader*> instacingModelShaders;
	vector<CUiShader*> instacingUiShaders;
	vector<CSkinedObInstancingShader*> instacingAnimatedModelShaders;

	CObInstancingShader* m_pCheckPointShader;

	CCamera* m_pMinimapCamera = NULL;

	BlurShader* blurShader = NULL;
};