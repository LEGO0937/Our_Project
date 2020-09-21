#pragma once

#include "BaseScene.h"

class CUiShader;


class LoadingScene : public BaseScene
{
public:
	LoadingScene();
	~LoadingScene();

	void ProcessInput(HWND hWnd, float deltaTime);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);

	void BuildObjects();
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables();
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	virtual void AnimateObjects(float fTimeElapsed);  //애니메이션 업데이트
	virtual SceneType Update(float fTimeElapsed);  // 충돌처리 및 물리

	void Render();

private:
	vector<CUiShader*> instacingUiShaders;
};
