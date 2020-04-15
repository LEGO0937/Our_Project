#pragma once
#include "BaseScene.h"

class CObInstancingShader;
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

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void AnimateObjects(float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);

private:
	vector<CObInstancingShader*> instacingShaders;
};