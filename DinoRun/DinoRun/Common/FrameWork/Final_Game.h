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
	virtual bool Initialize();     //������ �������� �ι�°�� ȣ��Ǵ� �Լ� dx����, �����츦 �ʱ�ȭ�Ѵ�.
	virtual void BuildObjects();   // �⺻���� �������� scene, player�� ������ ���� ��ü�� ����

	virtual void ReleaseObjects(); //�������� �޽��� ���� ��ü�� �Ҹ��ϴ� �Լ��̴�.  

	virtual void ProcessInput();
	virtual void AnimateObjects(); //scene�� �ִϸ��̼� ó��
	virtual void FrameAdvance();   // ������ update rander����� �ѹ��� ����ִ� �Լ�

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