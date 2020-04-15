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
	
	ComPtr<IDXGIFactory4> m_pdxgiFactory; //DXGI 팩토리 인터페이스에 대한 포인터이다.
	ComPtr<ID3D12Device> m_pd3dDevice; //Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ComPtr<IDXGISwapChain3> m_pdxgiSwapChain; //스왑 체인 인터페이스에 대한 포인터이다.주로 디스플레이를 제어하기 위하여 필요하다.

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0; //MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	
	static const UINT m_nSwapChainBuffers = 2; //스왑 체인의 후면 버퍼의 개수
	UINT m_nSwapChainBufferIndex; //현재 스왑 체인의 후면 버퍼 인덱스이다.
	
	ComPtr<ID3D12Resource> m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize; 
	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.
	
	ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize; 
	//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다.
	
	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList; //명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.
	
	ComPtr<ID3D12Fence> m_pd3dFence;
	UINT64 m_nFenceValue[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent; //펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.

	CGameTimer m_GameTimer; //게임 프레임워크에서 사용할 타이머이다.
	std::wstring mMainWndCaption = L"DinoRun";   //게임 이름 지정란

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
public:  //순수 가상함수들
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
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam); //윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	
public:
	CGameFramework(HINSTANCE hInstance);
	~CGameFramework();

	static CGameFramework* GetFramework(); //해당 프레임워크의 주소를 반환 

	HINSTANCE getInst()const;
	HWND      getMainWnd()const;
	float     AspectRatio()const;

	bool InitMainWindow();
	bool InitDirect3D();

	bool OnCreate(); //프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다).
	void OnDestroy();

	void CreateSwapChain();           //dx 도구 생성
	void CreateDirect3DDevice();	  //dx 도구 생성
	void CreateCommandQueueAndList(); //dx 도구 생성
	void CreateRtvAndDsvDescriptorHeaps();    //randerTarget와 depthStencil 힙 설정 
	void CreateRenderTargetView();	  //dx 도구 생성
	void CreateDepthStencilView();	  //dx 도구 생성

	int Run();
	void CalculateFrameStats();		  //윈도우에 프레임을 출력하는 함수

	void WaitForGpuComplete(); //CPU와 GPU를 동기화하는 함수이다. 

	void ChangeSwapChainState(); 
	void MoveToNextFrame();
};

