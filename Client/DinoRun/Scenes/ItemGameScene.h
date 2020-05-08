#pragma once
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
class MinimapShader;
class IconShader;

class CHeightMapTerrain;
class SkyBoxObject;


enum ItemIcon_type {IconDefault=0,IconBanana, IconStone,IconMud,IconMeat,IconMugen};

class ItemGameScene : public BaseScene
{
public:
	ItemGameScene();
	~ItemGameScene();

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

	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	void UpdateShadow();

	virtual void AnimateObjects(float fTimeElapsed);  //애니메이션 업데이트
	virtual SceneType Update(CreateManager* pCreateManager, float fTimeElapsed);  // 충돌처리 및 물리
	virtual void FixedUpdate(CreateManager* pCreateManager, float fTimeElapsed);

	void Render(float fTimeElapsed);
	void RenderShadow();
	virtual void RenderPostProcess(ComPtr<ID3D12Resource> curBuffer);

	virtual void ResetShadowBuffer(CreateManager* pCreateManager);

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

	CHeightMapTerrain* GetTerrain() { return m_pTerrain; }

private:
	ID3D12Resource *m_pd3dcbShadow = NULL;
	CB_GAME_INFO* m_pcbMappedShadow = NULL;

	SkyBoxObject* m_pSkyBox = NULL;
	CHeightMapTerrain* m_pTerrain = NULL;

	list<CObjectsShader*> UpdatedShaders;  //물리적용이 되는 셰이더를 담음

	vector<CObInstancingShader*> instacingBillBoardShaders;
	vector<CObInstancingShader*> instacingModelShaders;

	vector<CUiShader*> instacingNumberUiShaders;
	vector<CUiShader*> instacingImageUiShaders;
	CUiShader* m_pEffectShader = NULL;

	vector<CSkinedObInstancingShader*> instacingAnimatedModelShaders;

	CObInstancingShader* m_pCheckPointShader;

	CCamera* m_pMinimapCamera = NULL;

	BlurShader* blurShader = NULL;

	list<ParticleSystem*> particleSystems;

	MinimapShader* m_pMinimapShader = NULL;
	IconShader* m_pIconShader = NULL;

	ItemIcon_type m_eCurrentItem = ItemIcon_type::IconDefault;
};