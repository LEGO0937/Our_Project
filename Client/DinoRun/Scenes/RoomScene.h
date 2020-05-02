#pragma once

#include "BaseScene.h"

class CUiShader;


class RoomScene : public BaseScene
{
public:
	RoomScene();
	~RoomScene();

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
	virtual SceneType Update(float fTimeElapsed);  // 충돌처리 및 물리

	void Render(float fTimeElapsed);

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

private:
	vector<CUiShader*> instacingUiShaders;
	bool isClickedButton = false;

};
