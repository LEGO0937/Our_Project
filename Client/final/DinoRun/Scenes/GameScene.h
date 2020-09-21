#pragma once
#include "BaseScene.h"

#define TerrainScaleX 25.0f
#define TerrainScaleY 0.7f
#define TerrainScaleZ 25.0f

class CObInstancingShader;
class CSkinedObInstancingShader;
class CObjectsShader;
class CUiShader;
class MotionBlurShader;
class MinimapShader;
class IconShader;
class CountDownShader;

class CHeightMapTerrain;
class SkyBoxObject;


class GameScene : public BaseScene
{
public:
	GameScene();
	~GameScene();

	virtual void ReSize();

	void ProcessInput(HWND hWnd, float deltaTime);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);

	virtual void BuildLights();
	void BuildSubCameras();
	void ReBuildSubCameras();

	void BuildObjects();
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables();
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	void UpdateShadow();
	
	virtual void AnimateObjects(float fTimeElapsed);  //�ִϸ��̼� ������Ʈ
	virtual SceneType Update(float fTimeElapsed);  // �浹ó�� �� ����
	virtual void FixedUpdate(float fTimeElapsed);

	void Render();
	void RenderShadow();
	void RenderVelocity();
	virtual void RenderPostProcess(ComPtr<ID3D12Resource> curBuffer, ComPtr<ID3D12Resource> velocityMap);

	virtual void ResetShadowBuffer();

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

	CHeightMapTerrain* GetTerrain() { return m_pTerrain; }

	virtual void ProcessEvent(const MessageStruct& msg);
	virtual void ProcessPacket(char* packet, float fTimeElapsed);

	void UpdatePlayerInfo(char* packet, float fTimeElapsed);
	void UpdateEventInfo(char* packet, float fTimeElapsed);
	void UpdateInitInfo(char* packet, float fTimeElapsed);
	void UpdateStartInfo(char* packet, float fTimeElapsed);
	void UpdateFinishInfo(char* packet, float fTimeElapsed);

	void UpdatePlayerSliding(char* packet, float fTimeElapsed);
	void UpdatePlayerCollision(char* packet, float fTimeElapsed);
private:
	DWORD dwDirection;

	ID3D12Resource *m_pd3dcbShadow = NULL;
	CB_GAME_INFO* m_pcbMappedShadow = NULL;

	SkyBoxObject* m_pSkyBox = NULL;
	CHeightMapTerrain* m_pTerrain = NULL;

	vector<CObjectsShader*> UpdatedShaders;  //���������� �Ǵ� ���̴��� ����

	vector<CObInstancingShader*> instancingBillBoardShaders;
	vector<CObInstancingShader*> instancingModelShaders;

	vector<CUiShader*> instancingNumberUiShaders;
	vector<CUiShader*> instancingImageUiShaders;
	CUiShader* m_pEffectShader = NULL;
	CountDownShader* m_pCountDownShader = NULL;

	vector<CSkinedObInstancingShader*> instancingAnimatedModelShaders;

	CObInstancingShader* m_pCheckPointShader = NULL;
	CUiShader* m_pGuageShader = NULL;
	

	CCamera* m_pMinimapCamera = NULL;
	MotionBlurShader* motionBlurShader = NULL;

	list<ParticleSystem*> particleSystems;

	MinimapShader* m_pMinimapShader = NULL;
	IconShader* m_pIconShader = NULL;

	bool isAllConnected = false;
	bool isStart = false;

	float m_fCountDownTime = 0.0f;
};