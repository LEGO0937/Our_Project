#pragma once
/*
#include "../stdafx.h"
#include "CGameFramework.h"
#include "../../Scenes/GameScene.h"
#include "../../Objects/TerrainObject.h"

class Final_Game : public CGameFramework
{
private:
	GameScene* m_pgameScene;
public:
	Final_Game(HINSTANCE hInstance);
	~Final_Game();
public:
	virtual bool Initialize();     //생성자 다음으로 두번째로 호출되는 함수 dx도구, 윈도우를 초기화한다.
	virtual void BuildObjects();   // 기본도구 다음으로 scene, player등 게임을 위한 객체를 생성

	virtual void ReleaseObjects(); //렌더링할 메쉬와 게임 객체를 소멸하는 함수이다.  

	virtual void ProcessInput();
	virtual void AnimateObjects(); //scene의 애니메이션 처리
	virtual void FrameAdvance();   // 게임의 update rander기능을 한번에 담고있는 함수

	virtual void Clear_rt_and_ds();

	virtual ID3D12RootSignature *CreateGraphicsRootSignature();
	virtual void CreatePSOs();
	virtual void BuildPipelineState();

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	std::array<const D3D12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();
	CHeightMapTerrain* m_pTerrain = NULL;
};
*/