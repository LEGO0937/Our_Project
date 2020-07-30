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

	void BuildObjects(shared_ptr<CreateManager> pCreateManager);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	virtual void AnimateObjects(float fTimeElapsed);  //�ִϸ��̼� ������Ʈ
	virtual SceneType Update(CreateManager* pCreateManager, float fTimeElapsed);  // �浹ó�� �� ����

	void Render();

private:
	vector<CUiShader*> instacingUiShaders;
};
