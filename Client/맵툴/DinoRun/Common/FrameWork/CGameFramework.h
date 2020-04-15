#pragma once
#include "../stdafx.h"
#include "../Timer/Timer.h"
#include "../Camera/Camera.h"
#include "../../Scenes/BaseScene.h"
#include "../ImGui/imgui/imgui.h"
#include "../ImGui/imgui/examples/imgui_impl_win32.h"
#include "../ImGui//imgui/examples/imgui_impl_dx12.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class CGameFramework
{
protected:
	static CGameFramework* app;

	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth= FRAME_BUFFER_WIDTH;
	int m_nWndClientHeight= FRAME_BUFFER_HEIGHT;
	
	ComPtr<IDXGIFactory4> m_pdxgiFactory; //DXGI ���丮 �������̽��� ���� �������̴�.
	ComPtr<ID3D12Device> m_pd3dDevice; //Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ComPtr<IDXGISwapChain3> m_pdxgiSwapChain; //���� ü�� �������̽��� ���� �������̴�.�ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0; //MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.
	
	static const UINT m_nSwapChainBuffers = 2; //���� ü���� �ĸ� ������ ����
	UINT m_nSwapChainBufferIndex; //���� ���� ü���� �ĸ� ���� �ε����̴�.
	
	ComPtr<ID3D12Resource> m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize; 
	//���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ���̴�.
	
	ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize; 
	//����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ���̴�.
	
	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList; //��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� �������̴�.
	
	ComPtr<ID3D12Fence> m_pd3dFence;
	UINT64 m_nFenceValue[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent; //�潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.

	CGameTimer m_GameTimer; //���� �����ӿ�ũ���� ����� Ÿ�̸��̴�.
	std::wstring mMainWndCaption = L"DinoRun";   //���� �̸� ������

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
	int m_nPipelineStates = 0;
	static ID3D12PipelineState **m_ppd3dPipelineStates;

	ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = NULL;
#if defined(_DEBUG)
	ID3D12Debug *m_pd3dDebugController;
#endif
	POINT m_ptOldCursorPos;

	BaseScene* m_pCurrentScene;
public:
	CPlayer* m_pPlayer = NULL;
	CCamera* m_pCamera = NULL;
public:  //���� �����Լ���
	virtual bool Initialize();

	virtual void BuildObjects()=0;   
	virtual void ReleaseObjects()=0; 

	virtual void ProcessInput() =0;

	virtual void AnimateObjects() =0;
	virtual void FrameAdvance() =0;

 	virtual void Change_from_present_to_rt() = 0;
	virtual void Change_from_rt_to_present() = 0;
	virtual void Clear_rt_and_ds() = 0;

	virtual ID3D12RootSignature *CreateGraphicsRootSignature() = 0;
	virtual void CreatePSOs() = 0;
	virtual void BuildPipelineState() = 0;

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); //�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	
public:
	CGameFramework(HINSTANCE hInstance);
	~CGameFramework();

	static CGameFramework* GetFramework(); //�ش� �����ӿ�ũ�� �ּҸ� ��ȯ 

	HINSTANCE getInst()const;
	HWND      getMainWnd()const;
	float     AspectRatio()const;

	bool InitMainWindow();
	bool InitDirect3D();

	bool OnCreate(); //�����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�).
	void OnDestroy();

	void CreateSwapChain();           //dx ���� ����
	void CreateDirect3DDevice();	  //dx ���� ����
	void CreateCommandQueueAndList(); //dx ���� ����
	void CreateRtvAndDsvDescriptorHeaps();    //randerTarget�� depthStencil �� ���� 
	void CreateRenderTargetView();	  //dx ���� ����
	void CreateDepthStencilView();	  //dx ���� ����

	int Run();
	void CalculateFrameStats();		  //�����쿡 �������� ����ϴ� �Լ�

	void WaitForGpuComplete(); //CPU�� GPU�� ����ȭ�ϴ� �Լ��̴�. 

	void ChangeSwapChainState(); 
	void MoveToNextFrame();
};

