#pragma once
#include "BaseScene.h"
//#include "../CShaders/BlurShader/BlurShader.h"

#define TerrainScaleX 25.0f
#define TerrainScaleY 0.7f
#define TerrainScaleZ 25.0f

class CObInstancingShader;
class CSkinedObInstancingShader;
class CObjectsShader;
class CUiShader;
class BlurShader;
class MotionBlurShader;
class MinimapShader;
class IconShader;

class CHeightMapTerrain;
class SkyBoxObject;


class GameScene : public BaseScene
{
public:
	GameScene();
	~GameScene();

	virtual void ReSize(shared_ptr<CreateManager> pCreateManager);

	void ProcessInput(HWND hWnd, float deltaTime);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);

	virtual void BuildLights();
	void BuildSubCameras(shared_ptr<CreateManager> pCreateManager);
	void ReBuildSubCameras(shared_ptr<CreateManager> pCreateManager);

	void BuildObjects(shared_ptr<CreateManager> pCreateManager);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	void UpdateShadow();
	
	virtual void AnimateObjects(float fTimeElapsed);  //�ִϸ��̼� ������Ʈ
	virtual SceneType Update(CreateManager* pCreateManager, float fTimeElapsed);  // �浹ó�� �� ����
	virtual void FixedUpdate(CreateManager* pCreateManager, float fTimeElapsed);

	void Render();
	void RenderShadow();
	void RenderVelocity();
	virtual void RenderPostProcess(ComPtr<ID3D12Resource> curBuffer, ComPtr<ID3D12Resource> velocityMap);

	virtual void ResetShadowBuffer(CreateManager* pCreateManager);

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

	CHeightMapTerrain* GetTerrain() { return m_pTerrain; }

	virtual void ProcessEvent(const MessageStruct& msg);
	virtual void ProcessPacket(char* packet);
private:
	DWORD dwDirection;

	ID3D12Resource *m_pd3dcbShadow = NULL;
	CB_GAME_INFO* m_pcbMappedShadow = NULL;

	SkyBoxObject* m_pSkyBox = NULL;
	CHeightMapTerrain* m_pTerrain = NULL;

	list<CObjectsShader*> UpdatedShaders;  //���������� �Ǵ� ���̴��� ����

	vector<CObInstancingShader*> instancingBillBoardShaders;
	vector<CObInstancingShader*> instancingModelShaders;

	vector<CUiShader*> instancingNumberUiShaders;
	vector<CUiShader*> instancingImageUiShaders;
	CUiShader* m_pEffectShader = NULL;

	vector<CSkinedObInstancingShader*> instancingAnimatedModelShaders;

	CObInstancingShader* m_pCheckPointShader;
	CUiShader* m_pGuageShader;

	CCamera* m_pMinimapCamera = NULL;

	BlurShader* blurShader = NULL;
	MotionBlurShader* motionBlurShader = NULL;

	list<ParticleSystem*> particleSystems;

	MinimapShader* m_pMinimapShader = NULL;
	IconShader* m_pIconShader = NULL;
	bool isStart = false;
};