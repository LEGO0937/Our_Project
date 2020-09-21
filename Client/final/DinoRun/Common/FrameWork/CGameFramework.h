#pragma once
#include "../stdafx.h"
#include "../Timer/Timer.h"
#include "../Camera/Camera.h"
#include "../../Scenes/BaseScene.h"
#include "FontManager.h"

class CGameFramework
{

private: 
	string m_sPlayerID = "";
public:
	CGameFramework();
	~CGameFramework();
public:
	bool Initialize(HINSTANCE hInstance, HWND hWnd);
	void Release();

	void FrameAdvance();
	void BuildObjects();

	void CalculateFrameStats();

	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingPacket(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);

	bool IsRunning() { return m_running; }

	void BuildPipelineState();
	void CreatePSOs();

private: // 내부 함수
	void ReleaseObjects();

	void ChangeSceneByType(SceneType type);

private: // 변수
	HWND m_hWnd{ NULL };
	shared_ptr<FontManager> m_pFontManager;

	shared_ptr<BaseScene> m_pScene;
	shared_ptr<BaseScene> m_pLoadingScene;


	bool m_running{ true };
	
	POINT m_ptOldCursorPos;
	SceneType m_CurState = Start_Scene;
	SceneType m_PrevState = Start_Scene;

	CGameTimer m_GameTimer; //게임 프레임워크에서 사용할 타이머이다.
	CPlayer* m_pPlayer = NULL;
	CCamera* m_pCamera = NULL;

	int m_nPipelineStates = 0;
	static ID3D12PipelineState **m_ppd3dPipelineStates;
};

