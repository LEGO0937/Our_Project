#include "Final_Game.h"
/*


Final_Game::Final_Game(HINSTANCE hInstance) :CGameFramework(hInstance)
{

}

Final_Game::~Final_Game()
{

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	Final_Game theApp(hInstance);

	if (!theApp.Initialize())
		return 0;

	return theApp.Run();
}

bool Final_Game::Initialize()
{
	if (!CGameFramework::Initialize())
		return false;
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), nullptr);
	HRESULT hResult = m_pd3dCommandList->Close();
	return true;
}

void Final_Game::BuildObjects()
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature();
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);
	BuildPipelineState();

	m_pgameScene = new GameScene;
	m_pgameScene->SetPipelineStates(m_nPipelineStates, m_ppd3dPipelineStates);
	m_pgameScene->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	m_pgameScene->BuildObjects(m_pd3dDevice.Get(),m_pd3dCommandList.Get());

	CTerrainPlayer *pPlayer = new CTerrainPlayer(m_pd3dDevice.Get(), m_pd3dCommandList.Get(), m_pd3dGraphicsRootSignature, m_pgameScene->m_pTerrain);
	
	m_pgameScene->m_pPlayer = m_pPlayer = pPlayer;
	m_pgameScene->m_pCamera = m_pCamera = m_pPlayer->GetCamera();
	
	m_pCurrentScene = m_pgameScene;

	//Scene_stack.reserve(5);  //시작상태, 플레이상태, 일시정지 현재는 씬은 세가지 생각 중
	//Scene_stack.emplace_back(new GameScene());
	//if (Scene_stack.size() != 0)
	//{
	//	Scene_stack[Scene_stack.size() - 1]->BuildObjects(m_pd3dDevice, m_pd3dCommandList, m_pd3dDsvDescriptorHeap);
	//}

	//u_Player = new CAirplanePlayer(m_pd3dDevice,
	//	m_pd3dCommandList, Scene_stack[Scene_stack.size() - 1]->GetGraphicsRootSignature());
	//원래는 시작씬을 시작으로 시작씬에서 플레이어 정보를 서버로부터 받고 플레이어객체를 리턴해서 프레임워크에
	//데이터를 넣기로 해야함
	//Scene_stack[Scene_stack.size() - 1]->setPlayer(u_Player);
	//Scene_stack[Scene_stack.size() - 1]->setCamera(u_Player->GetCamera());
	//player객체와  씬객체 할당해제할것
	
	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	WaitForGpuComplete();
	if (m_pgameScene) 
		m_pgameScene->ReleaseUploadBuffers();
	//if (Scene_stack.size() != 0)
	//	Scene_stack[Scene_stack.size() - 1]->ReleaseUploadBuffers();
	if(m_pPlayer)
		m_pPlayer->ReleaseUploadBuffers();
}

void Final_Game::ReleaseObjects()
{
	//for (BaseScene* v : Scene_stack)
	//{
	//	v->ReleaseShaderVariables();
	//	v->ReleaseObjects();
	//	delete v;
	//}

	if (m_pPlayer)
	{
		m_pPlayer->Release();
	}
	if(m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();

	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i])
			m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
	if (m_pgameScene)
	{
		m_pgameScene->ReleaseObjects();
		m_pgameScene->ReleaseShaderVariables();
		delete m_pgameScene;
		m_pgameScene = NULL;
	}
}

void Final_Game::ProcessInput()
{
	//Scene_stack[Scene_stack.size() - 1]->ProcessInput(m_hWnd, m_GameTimer.DeltaTime());
	m_pgameScene->ProcessInput(m_hWnd, m_GameTimer.DeltaTime());
}

void Final_Game::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.DeltaTime();
	if (m_pPlayer)
		m_pPlayer->Animate(fTimeElapsed);
	//if (!Scene_stack.empty())
	//	Scene_stack[Scene_stack.size() - 1]->AnimateObjects(m_GameTimer.DeltaTime());
}

void Final_Game::FrameAdvance()  //이곳에 update, rander 기능 모두 포함돼있음
{
	m_GameTimer.Tick();

	ProcessInput();  //키 입력

	AnimateObjects();

	m_pd3dCommandAllocator->Reset();
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	//후면버퍼를 present에서 랜더타겟으로 변환

	
	ChangeResourceState(m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Clear_rt_and_ds();    //랜더타겟,깊이스텐실 clear

	//if (!Scene_stack.empty())
	//	Scene_stack[Scene_stack.size() - 1]->Render(m_pd3dCommandList);

	//3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다. 
#ifdef _WITH_PLAYER_TOP
	//렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다. 
	m_pd3dCommandList->Clear

		StencilView(d3dDsvCPUDescriptorHandle,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	//3인칭 카메라일 때 플레이어를 렌더링한다. 
	//if (m_pPlayer) m_pPlayer->Render(m_pd3dCommandList, m_pCamera);
	if (m_pgameScene)
		m_pgameScene->Render(m_pd3dCommandList.Get());
	
	ChangeResourceState(m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	ExecuteCommandList();
	m_pdxgiSwapChain->Present(0, 0);
	MoveToNextFrame();

}

void Final_Game::Clear_rt_and_ds()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex *
		m_nRtvDescriptorIncrementSize);

	float pfClearColor[4] = { 0.0f, 0.2f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle,
		pfClearColor, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE,
		&d3dDsvCPUDescriptorHandle);
}

//정적 샘플러
array<const D3D12_STATIC_SAMPLER_DESC, 7> Final_Game::GetStaticSamplers()
{
	const D3D12_STATIC_SAMPLER_DESC pointWrap{ D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 0, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC pointClamp{ D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 1, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC linearWrap{ D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 2, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC linearClamp{ D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 3, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC anisotropicWrap
	{ D3D12_FILTER_ANISOTROPIC,
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	0.0f, 8, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
	, 0.f, D3D12_FLOAT32_MAX, 4, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC anisotropicClamp
	{ D3D12_FILTER_ANISOTROPIC,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	0.0f, 8, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
	, 0.f, D3D12_FLOAT32_MAX, 5, 0, D3D12_SHADER_VISIBILITY_ALL
	};
	const D3D12_STATIC_SAMPLER_DESC shadow{
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK, 0.f, D3D12_FLOAT32_MAX, 6, 0, D3D12_SHADER_VISIBILITY_ALL };


	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp,shadow };
}

ID3D12RootSignature *Final_Game::CreateGraphicsRootSignature()
{
	D3D12_DESCRIPTOR_RANGE texTable;
	texTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	texTable.NumDescriptors = 2;
	texTable.BaseShaderRegister = 1;
	texTable.RegisterSpace = 0;
	texTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[3];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 2;
	pd3dDescriptorRanges[0].BaseShaderRegister = 3; //Game Objects
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 1; //Diffuse
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 2; //Detail
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;


	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[10];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Game Object   상수 버퍼용
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[2].Constants.Num32BitValues = 32;
	pd3dRootParameters[2].Constants.ShaderRegister = 5; //인스턴싱이 아닌 물체들을 사용
	pd3dRootParameters[2].Constants.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 0; //t0   인스턴싱버퍼 묶음
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 1; //Materials   메터리얼 버퍼
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 2; //Lights     라이트 버퍼
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[6].Descriptor.ShaderRegister = 7; //Skinned Bone Offsets
	pd3dRootParameters[6].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[7].Descriptor.ShaderRegister = 8; //Skinned Bone Transforms
	pd3dRootParameters[7].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //Base Texture
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2]; //Detail Texture
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	auto staticSamplers = GetStaticSamplers();

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = staticSamplers.size();
	d3dRootSignatureDesc.pStaticSamplers = staticSamplers.data();
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	
	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	
	HRESULT h = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	m_pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	return(pd3dGraphicsRootSignature);
	
}
void Final_Game::BuildPipelineState()
{
	m_nPipelineStates = 9;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppd3dPipelineStates[i] = NULL;
	}
	CreatePSOs();
}
void Final_Game::CreatePSOs()
{
	CreatePsoLighting(m_pd3dDevice.Get(), m_pd3dGraphicsRootSignature, m_ppd3dPipelineStates, 0);
	CreatePsoInstancing(m_pd3dDevice.Get(), m_pd3dGraphicsRootSignature, m_ppd3dPipelineStates, 4);
	CreatePsoTexting(m_pd3dDevice.Get(), m_pd3dGraphicsRootSignature, m_ppd3dPipelineStates, 1);
	CreatePsoTerrain(m_pd3dDevice.Get(), m_pd3dGraphicsRootSignature, m_ppd3dPipelineStates, 7);
	CreatePsoUi(m_pd3dDevice.Get(), m_pd3dGraphicsRootSignature, m_ppd3dPipelineStates, 5);
	CreatePsoCube(m_pd3dDevice.Get(), m_pd3dGraphicsRootSignature, m_ppd3dPipelineStates, 2);
	CreatePsoTextedInstancing(m_pd3dDevice.Get(), m_pd3dGraphicsRootSignature, m_ppd3dPipelineStates, 3);
	CreatePsoSurf(m_pd3dDevice.Get(), m_pd3dGraphicsRootSignature, m_ppd3dPipelineStates, 6);
	CreatePsoSkinMesh(m_pd3dDevice.Get(), m_pd3dGraphicsRootSignature, m_ppd3dPipelineStates, 8);
}

void Final_Game::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pgameScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam,m_GameTimer.DeltaTime());
	//Scene_stack[Scene_stack.size() - 1]->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam, m_GameTimer.DeltaTime());
}

void Final_Game::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_F9:
			ChangeSwapChainState();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	m_pgameScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam, m_GameTimer.DeltaTime());
	//Scene_stack[Scene_stack.size() - 1]->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam, m_GameTimer.DeltaTime());
}

LRESULT Final_Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)    //메시지 처리함수
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingWindowMessage(hwnd, msg, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
*/