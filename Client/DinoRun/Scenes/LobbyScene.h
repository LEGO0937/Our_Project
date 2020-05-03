#pragma once
#include "BaseScene.h"

class CObInstancingShader;
class CUiShader;
class CObjectsShader;

class LobbyScene : public BaseScene
{
public:
	LobbyScene();
	~LobbyScene();

	void ProcessInput(HWND hWnd, float deltaTime);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);

	void BuildObjects(CreateManager* pCreateManager);

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	virtual void AnimateObjects(float fTimeElapsed);  //애니메이션 업데이트
	virtual SceneType Update(CreateManager* pCreateManager, float fTimeElapsed);  // 충돌처리 및 물리

	void Render(float fTimeElapsed);

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

private:
	vector<CObInstancingShader*> instacingBillBoardShaders;
	vector<CUiShader*> instacingUiShaders;

	bool isClickedLeftRoom = false;
	bool isClickedRightRoom = false;
	bool isClickedLeftUser = false;
	bool isClickedRightUser = false;

	bool isClicked = false;
	float m_fClickedTime = 0.f;
};
