//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"
#include "DDSTextureLoader12.h"


#define TEXTURES 4

CShader::CShader()
{
	m_d3dSrvCPUDescriptorStartHandle.ptr = NULL;
	m_d3dSrvGPUDescriptorStartHandle.ptr = NULL;
}

CShader::~CShader()
{
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pd3dErrorBlob = NULL;
	::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, NULL);
	//	char *pErrorString = (char *)pd3dErrorBlob->GetBufferPointer();

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;


	//아래의 정의되어있는 Create...함수들은 모두 오버라이딩이 가능한 객체들이다
	//그렇기 때문에 CShader를 상속받은 자식 클래스들은 모두 자신의 것으로 재정의 가능하다.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;	//이 구조체는 그래픽스 파이프라인이 어떤식으로 동작할것인지를 정의하는 구조체이다. 
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;			//루트 시그니쳐는 리소스들을 몇번에 연결하고 셰이더에 몇번 레지스터에 넘겨줄것인가를 정의한다.
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);		//이 함수로 만약 오버라이딩이 되어있으면 오버라이딩한 정점셰이더가 호출될것이다.	
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);			//픽셀쉐이더도 정점쉐이더와 동일
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();				//
	d3dPipelineStateDesc.BlendState = CreateBlendState();						
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();						//입력조립기를 정의
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;		//정점들을 조립할때 Triangle_list,TriangleStrip등으로 어떻게 조립할지를 정한다.
	d3dPipelineStateDesc.NumRenderTargets = 1;				//렌더타겟의 개수를 정의한다. 후처리 할때 렌더타겟이 여러개 필요할 경우가 생기겠지만 서버는 알필요 없을듯
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		//렌더타겟의 각 픽셀의 들어가야할 값
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;			//깊이 버퍼의 형식(즉 , 들어가야 할 값)
	d3dPipelineStateDesc.SampleDesc.Count = 1;								//서버는 신경 쓸 필요 없음
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_pd3dPipelineState);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{
	//디스크립터 힙이란 간단히 생각하는게 이해하기 편할것이다.
	// 쉽게 말하자면, 리소스에 접근하려면 디스크립터(서술자)가 필요하다.
	// 이 서술자들을 이 힙의 넣어놓고 해당 객체를 그릴때 이 힙의 서술자들을 이용해서 리소스의 접근함.
	// 이 힙에 CBV , SRV 를 각 배열의 원소마다 할당한다.
	// CBV는 ConstantBufferView 약자로써 셰이더에서 전역 변수로 쓰일 변수들이다.
	// SRV는 ShaderResourceView의 약자로써 주로 텍스처들을 쉐이더에 넘길때 사용된다.
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	
	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);

}

//물론 이부분도 서버가 알 필요는 없어 보임
//CBV를 생성하는 함수
//이 함수가 호출되기 전에 무조건 디스크립터 힙이 만들어 져 있어야한다. 
void CShader::CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();	//디스크립터 힙 내에 해당 cbv가 위치하고 있는 주소를 반환하여 저장.
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;					
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

//ShaderResourceView를 생성
D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;		//뷰가 리소스를 보는 형식
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	//텍스쳐의 rgba를 읽는 순서를 정해준다.
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY:
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = 0;
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

void CShader::CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement)
{
	/*D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;*/
	int nTextures = pTexture->GetTextures();
	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource *pShaderResource = pTexture->GetTexture(i);				//텍스처 리소스를 하나 얻어온다.
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();		//그 리소스들에 대한 정보를 얻어온다.
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);  //텍스처에 대한 서술자 뷰를 설정한다.
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorStartHandle);		//cpu가 접근할수있는 srv뷰를 만든다.
		m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		//srv뷰를 
		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, m_d3dSrvGPUDescriptorStartHandle);
		m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

void CShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
}

void CShader::ReleaseShaderVariables()
{
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
}

void CShader::ReleaseUploadBuffers()
{
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{
	// 가장 먼저 SetGraphicsRootSignature 선언이 필요하다 .
	// 하지만 CScene::Render함수 맨 처음에 이미 선언을 해줬으므로 다시 set해줄 필요는 없다.
	// 하지만 씬이 관리하는 쉐이더가 아니라면(즉 , 후처리 쉐이더라면) 
	// 따로 SetGraphicsRootSignature선언을 해줘야 할것이다.
	// 렌더 과정 1.
	//pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	
	// 렌더 과정 2. 
	// CreateShader 함수에서 생성한 파이프라인 스테이트를 
	// SetPipelineState()로 set해준다. (이 과정을 통해서 VS,PS등의 쉐이더연결)

	if (m_pd3dPipelineState) pd3dCommandList->SetPipelineState(m_pd3dPipelineState);	//그리기 시작
	
	// 렌더 과정 3.
	// 루트상수가 아닌 cbv,srv, 테이블에 있는 서술자들은 
	// SetDescriptorHeaps()로 set을 해줘야 한다. 
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);				//camera


	// 이 함수는 CShader를 상속한 자식 클래스들이 만약 이 함수를 오버라이딩 했으면
	// 오버라이딩한 함수가 대신 호출됨
	UpdateShaderVariables(pd3dCommandList);
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

//입력조립기 CPlayerShader에서 오버라이딩 했음 
D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{

	//입력조립기로 각 정점을 만들때 정점에 들어갈 정보들을 여기서 만들어준다. 
	// 현재 플레이어를 구성하는 정점들은 위치값과 색상정보를 갖는다.
	// 이 정점들은 모델좌표계로 구성되어 있기 때문에  쉐이더 파일 내에서 상수 버퍼뷰로 넘겨준 월드 변환 행렬, 뷰변환 행렬, 투영변환 행렬과 곱하여져서 물체는 월드 공간에 놓여지고 
	// 쉐이더 파일 내에서 좌표계변환이 이루어진다.  //즉 cpu가 하는일은 모델 좌표계의 정점을 쉐이더에 넘겨주는것.
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] ={ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] ={ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}
D3D12_DEPTH_STENCIL_DESC CPlayerShader::CreateDepthStencilState()
{
	//서버가 알 필요 없음
	// 깊이 스텐실 구조체
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;								
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	//Shaders.hlsl내의 VSPlayer함수를 컴파일해준다. 오버라이딩한 VSPlayer를 컴파일.
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSPlayer", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	//Shaders.hlsl 내의 PSPlayer 함수를 컴파일 해준다. 
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSPlayer", "ps_5_1", ppd3dShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	//오버라이딩한 함수들이 있으면 오버라이딩한 함수들을 GRAPHICS_PIPELINE_STATE의 연결해준다.
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexturedShader::CTexturedShader()
{
}

CTexturedShader::~CTexturedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTexturedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];




	pd3dInputElementDescs[0] ={ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] ={ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTexturedShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTextured", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTexturedShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTextured", "ps_5_1", ppd3dShaderBlob));
}

void CTexturedShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] ={ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] ={ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] ={ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD" ,1, DXGI_FORMAT_R32G32_FLOAT,0,36,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	


	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTerrain", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTerrain", "ps_5_1", ppd3dShaderBlob));
}

void CTerrainShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}



///////////////////////////////////////////////////////////////////////////////////////
//billboardShader
CBillboardShader::CBillboardShader()
{

}

CBillboardShader::~CBillboardShader()
{

}

void CBillboardShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

																		//리소스와 내부적인 힙을 생성 
																		//D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER -> 정점 버퍼 또는 상수 버퍼로 사용될때 초기 STATE

																		//업로드힙
	m_pd3dcbBillboardObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	//리소스에 대한 CPU포인터를 반환 (다중 - 쓰레드 안전)
	m_pd3dcbBillboardObjects->Map(0, NULL, (void **)&m_pcbMappedBillboardObjects);	//m_pcbMappedGameObjects 는 영구적으로 cpu가 접근 가능한 주소가 된다. 이 리소스에 대한 포인터는 영구적으로 위치를 안바꾼다.
																			//왜냐하면 cpu가 자주 접근하는 리소스이기 때문에
																			//m_pcbMappedGameObjects는 읽기를 하지 말아야한다.(성능상 문제가 발생)

}

void CBillboardShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{

	// 이 함수도 CShader::UpdateShaderVariables 함수를 오버라이딩 한것으로써
	// CreateShaderVariables에서 상수버퍼로 비디오 메모리의 할당을 해놨다면  
	// UpdateShaderVaribles함수로 실제 이 함수에서 각 객체들마다 갖고있는 월드 좌표값을 

	// Object.h 전역변수로 선언되어있는 아래의 구조체에 연결해준다. 
	// 이 구조체는 Shaders.hlsl에 cbGameObjectInfo에 추후에 연결 시켜 줄것이다. 
	//struct CB_GAMEOBJECT_INFO
	//{
	//	XMFLOAT4X4						m_xmf4x4World;
	//	UINT							m_nObjectID;
	//};

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nObjects; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedBillboardObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));

	}

}

void CBillboardShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CBillboardShader::ReleaseShaderVariables()
{
	if (m_pd3dcbBillboardObjects)
	{
		m_pd3dcbBillboardObjects->Unmap(0, NULL);
		m_pd3dcbBillboardObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

void CBillboardShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CBillboardShader::AnimateObjects(float fTimeElapsed,CCamera *pCamera,CPlayer* pPlayer)
{
	
	for (int j = 0; j < m_nObjects; j++)
	{

		m_ppObjects[j]->Animate(fTimeElapsed,pCamera);
		

	}
}

void CBillboardShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{

	//쉐이더 클래스에서 배치 처리해야할 오브젝트들을 생성하는 함수

	//빌보드는 지형위에 위치해야하기 때문에 지형의 정보를 받아와야한다. 

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;



	//이 변수들은 각 오브젝트가 얼마만큼 거리에 떨어져서 생성될것이냐를 결정하는데 
	// 이것은 알 필요 없다. 어차피 졸작에서 이런식으로 안할 것이기 때문에 
	// 이유는 부자연스럽게  규칙적으로 되어있기 때문
	float fxPitch = 100.0f;				//이 값이 커질수록 오브젝트의 수가 적어진다. fxPitch 지형의 x좌표 변화량
	float fyPitch = 1.0f;
	float fzPitch = 100.0f;				//

	//지형의 가로길이 세로길이를 받아온다. 
	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();


	// 각 지형위의 실제 위치해야할 오브젝트 수를 결정한다. 
	int xObjects = int(fTerrainWidth / fxPitch);
	int yObjects = 1;
	int zObjects = int(fTerrainLength / fzPitch);
	//CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	//pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Tree01s.dds", 0);




	// 이 부분이 dds파일을 로드하는 부분이다. 
	// 서버는 자세히 알필요는 없어 보인다 현재로서는
	CTexture *ppTextures[TEXTURES];
	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[0]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/tree01s.dds", 0);
	ppTextures[1] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[1]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/tree02S.dds", 0);
	ppTextures[2] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[2]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/tree35S.dds", 0);
	ppTextures[3] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTextures[3]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/tree4.dds", 0);



	/*CTexture *pTextureArray = new CTexture(1, RESOURCE_TEXTURE2DARRAY, 0);
	pTextureArray->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/treearray.dds", 0);*/


	m_nObjects = (xObjects * yObjects * zObjects);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);




	//이부분에는 순서가 중요하다. 
	// 디스크립터 힙 생성 - 상수 버퍼뷰를 사용한다면 비디오 메모리에 리소스 생성 - 뷰 연결
	// 이 순서이다. 
	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, TEXTURES);				//m_nObjects만큼의 상수 버퍼뷰와 1개의 srv
	CreateShaderVariables(pd3dDevice, pd3dCommandList);										//업로드힙에 리소스 생성
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_pd3dcbBillboardObjects, ncbElementBytes); //오브젝트를 상수 버퍼뷰 생성
	
	// Srv를 생성하고 루트 파라미터에 연결해준다 .
	// 마지막 bool인자가 true, false 마다 동작이 다르지만 생략하겠다. 
	for (int i = 0; i < TEXTURES; i++) {
		CreateShaderResourceViews(pd3dDevice, pd3dCommandList, ppTextures[i], 8, false);				//SRV 버퍼 뷰 생성
	}
	

	CMaterial *ppMaterials[TEXTURES];
	for (int i = 0; i < TEXTURES; i++)
	{
		ppMaterials[i] = new CMaterial();
		ppMaterials[i]->SetTexture(ppTextures[i]);
	}


	// 빌보드 메시를 생성하고 SetMesh함수를 이용해 각 오브젝트의 메쉬를 set해준다. 
	//CCubeMeshTextured *pCubeMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	CTexturedRectMesh* pBillboardMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 50.0f, 50.0f, 0.0f,0.0f,0.0f,-10.0f );

	m_ppObjects = new CGameObject*[m_nObjects];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CBillboardObject* pBillboardObject = NULL;

	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z< zObjects; z++)
		{

				
				//////////////////////////////////////////////////////////////////
				//이 구간을 지형의 기울기를 읽어와서 
				//지형이 평평하다고 생각되면
				//빌보드를 셋할 예정
				pBillboardObject = new CBillboardObject();
				pBillboardObject->SetMesh(0, pBillboardMesh);
//#ifndef _WITH_BATCH_MATERIAL
//				pBillboardObject->SetMaterial(pCubeMaterial);
//#endif
				pBillboardObject->SetMaterial(ppMaterials[i % TEXTURES]);	//
				float xPosition = x * fxPitch;
				float zPosition = z * fzPitch;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);		//여기서 지형의 위치를 받아온다.
				if (fHeight > 100.0f) {
					pBillboardObject->SetPosition(xPosition, fHeight + 18.0f, zPosition);	//빌보드 위치 결정
				}
				//////////////////////////////////////////////////////////
				//여기아래는 로직이 같으므로 건드릴필요 없어보임

		
				xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
				xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
				if (Vector3::IsZero(xmf3RotateAxis))
				{
					xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}

				float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
				pBillboardObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
			
				//gpu도 해당 포인터를 갖고있어야 하기때문에 반드시 아래 함수를 호출해야함. 
				pBillboardObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize*i));
				m_ppObjects[i++] = pBillboardObject;
			
			
		}
	}

}

void CBillboardShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	//
	CTexturedShader::Render(pd3dCommandList, pCamera);
/////////////////////////////////////////////////////////////////////////////////

	//위의 함수로 아래 과정이 실행된것이라 생각하면됨. 
	// CShader::OnPrepareRender 참고

	//pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	//if (m_pd3dPipelineState) pd3dCommandList->SetPipelineState(m_pd3dPipelineState);	//그리기 시작
	//pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);				//camera
	//UpdateShaderVariables(pd3dCommandList);

/////////////////////////////////////////////////////////////////////////////////

	//위의 과정이 실행 되면 
	// 배치 처리의 기본적인 준비가 된것이라 생각해도된다.
	// 아래의 for문은 각 오브젝트 하나하나를 그리는 과정이다. 
	// Render() 함수를 들어가보면
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList, pCamera,j);
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//CGrassBillboardShader 
//
CGrassBillboardShader::CGrassBillboardShader()
{

}

CGrassBillboardShader::~CGrassBillboardShader()
{}

D3D12_SHADER_BYTECODE CGrassBillboardShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSGrassTextured", "ps_5_1", ppd3dShaderBlob));
}

void CGrassBillboardShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

																		//리소스와 내부적인 힙을 생성 
																		//D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER -> 정점 버퍼 또는 상수 버퍼로 사용될때 초기 STATE
																		//업로드힙
	m_pd3dcbBillboardObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	//리소스에 대한 CPU포인터를 반환 (다중 - 쓰레드 안전)
	m_pd3dcbBillboardObjects->Map(0, NULL, (void **)&m_pcbMappedBillboardObjects);	//m_pcbMappedGameObjects 는 영구적으로 cpu가 접근 가능한 주소가 된다. 이 리소스에 대한 포인터는 영구적으로 위치를 안바꾼다.
																					//왜냐하면 cpu가 자주 접근하는 리소스이기 때문에

}

void CGrassBillboardShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{

	// 이 함수도 CShader::UpdateShaderVariables 함수를 오버라이딩 한것으로써
	// CreateShaderVariables에서 상수버퍼로 비디오 메모리의 할당을 해놨다면  
	// UpdateShaderVaribles함수로 실제 이 함수에서 각 객체들마다 갖고있는 월드 좌표값을 

	// Object.h 전역변수로 선언되어있는 아래의 구조체에 연결해준다. 
	// 이 구조체는 Shaders.hlsl에 cbGameObjectInfo에 추후에 연결 시켜 줄것이다. 
	//struct CB_GAMEOBJECT_INFO
	//{
	//	XMFLOAT4X4						m_xmf4x4World;
	//	UINT							m_nObjectID;
	//};
	

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	

	for (int j = 0; j < m_nObjects; j++)
	{

		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedBillboardObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));

	}


}


void CGrassBillboardShader::ReleaseShaderVariables()
{
	if (m_pd3dcbBillboardObjects)
	{
		m_pd3dcbBillboardObjects->Unmap(0, NULL);
		m_pd3dcbBillboardObjects->Release();
	}



	CTexturedShader::ReleaseShaderVariables();
}

void CGrassBillboardShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif

}

void CGrassBillboardShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera = NULL)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList, pCamera,j);
	}
}

void CGrassBillboardShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CGrassBillboardShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{


	//쉐이더 클래스에서 배치 처리해야할 오브젝트들을 생성하는 함수

	//빌보드는 지형위에 위치해야하기 때문에 지형의 정보를 받아와야한다. 
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;


	//이 변수들은 각 오브젝트가 얼마만큼 거리에 떨어져서 생성될것이냐를 결정하는데 
	// 이것은 알 필요 없다. 어차피 졸작에서 이런식으로 안할 것이기 때문에 
	// 이유는 부자연스럽게  규칙적으로 되어있기 때문
	float fxPitch = 50.0f;				//이 값이 커질수록 오브젝트의 수가 적어진다. fxPitch 지형의 x좌표 변화량
	float fyPitch = 1;
	float fzPitch = 50.0f;				//

	//지형의 가로길이 세로길이를 받아온다. 
	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	// 각 지형위의 실제 위치해야할 오브젝트 수를 결정한다. 
	int xObjects = int(fTerrainWidth / fxPitch);
	int yObjects = 1;
	int zObjects = int(fTerrainLength / fzPitch);

	// 이 부분이 dds파일을 로드하는 부분이다. 
	// 서버는 자세히 알필요는 없어 보인다 현재로서는

	CTexture *ppTexture[3];
	ppTexture[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTexture[0]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/grass01.dds", 0);
	ppTexture[1] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTexture[1]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/Grass02.dds", 0);
	ppTexture[2] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	ppTexture[2]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Texture/grass04.dds", 0);


	m_nObjects = (xObjects * yObjects * zObjects);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	//이부분에는 순서가 중요하다. 
	// 디스크립터 힙 생성 - 상수 버퍼뷰를 사용한다면 비디오 메모리에 리소스 생성 - 뷰 연결
	// 이 순서이다. 

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 3);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);										//업로드힙에 리소스 생성
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_pd3dcbBillboardObjects, ncbElementBytes); //오브젝트를 상수 버퍼뷰 생성
	
	
	//7번 파라미터에 위의 세 dds 텍스쳐 연결해주고 있는것. (즉 , 한 파라미터만 사용하는것이다)
	// 이것까지 자세히 알필요 없어보임
	for (int i = 0; i < 3; i++) {
		CreateShaderResourceViews(pd3dDevice, pd3dCommandList, ppTexture[i], 7, false);				//SRV 버퍼 뷰 생성
	}
//#ifdef _WITH_BATCH_MATERIAL
//	m_pMaterial = new CMaterial();
//	m_pMaterial->SetTexture(pTexture);
//#else
//	CMaterial *pCubeMaterial = new CMaterial();
//	pCubeMaterial->SetTexture(pTexture);
//#endif
	CMaterial *ppMaterials[3];
	for (int i = 0; i < 3; i++)
	{
		ppMaterials[i] = new CMaterial();
		ppMaterials[i]->SetTexture(ppTexture[i]);
	}



	CTexturedRectMesh* pBillboardMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 15.0f, 15.0f, 0.0f, 0.0f, 0.0f, -10.0f);

	m_ppObjects = new CGameObject*[m_nObjects];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CGrassBillboardObject* pBillboardObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z< zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++) {
				pBillboardObject = new CGrassBillboardObject();
				pBillboardObject->SetMesh(0, pBillboardMesh);
//#ifndef _WITH_BATCH_MATERIAL
//				pBillboardObject->SetMaterial(pCubeMaterial);
//#endif

				//pBillboardObject->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 1.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
				pBillboardObject->SetMaterial(ppMaterials[i % 3]);
				float xPosition = x * fxPitch + (rand()%10);
				float zPosition = z * fzPitch + (rand()%11); 
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);

				pBillboardObject->SetPosition(xPosition, fHeight + (y * 3.0f * fyPitch)+8.0f , zPosition);
				pBillboardObject->SetOOBB(XMFLOAT3(xPosition, fHeight, zPosition), XMFLOAT3(2.0f, 1.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
				//std::cout << pBillboardObject->GetPosition().x << pBillboardObject->GetPosition().y << pBillboardObject->GetPosition().z << std::endl;
				if (y == 0) {
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis))
					{
						xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
					}

					float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
					pBillboardObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}

				pBillboardObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize*i));
				m_ppObjects[i++] = pBillboardObject;
			}
		}


	}
}

void CGrassBillboardShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera,CPlayer *pPlayer)
{

	// 이 ContaintmentType ENUM형으로써 충돌 검사에 쓰일 자료형을 말한다. 
	//  DISJOINT = 0,INTERSECTS = 1, CONTAINS = 2, 가 있고 
	// DISJOINT 는 아예 겹치지 않은 상태로 충돌하지 않은 상태이다.
	// INTERSECTS 는 딱 충돌 된 상태를 의미 삼각형으로 따지면 테두리에 맞은 상태라 할 수 있겠다.
	// CONTAINS 는 도형 내부에 위치한 상태를 의미한다. 
	ContainmentType containType = DISJOINT;
	for (int j = 0; j < m_nObjects; j++)
	{
		//Bounding Box나 Boundind Oriented Box 같은 충돌 구체들은 
		// Contains함수나 Intersects 함수를 제공해 충돌검사를 용이하게 도와준다. 
		containType=pPlayer->m_sphereBox->Contains(m_ppObjects[j]->m_xmOOBB);

		// 아래 코드는 풀잎이 플레이어의 구체(눈에 보이진 않지만 플레이어를 중심으로 반지름 100m의 Bounding Box가 있다)
		// CPlayer생성자 참고 바람. 
		// 
		if (containType == CONTAINS)
		{
			m_ppObjects[j]->Animate(fTimeElapsed, pCamera, true);	//true일경우 플레이어 반경 100m의 들어온것
		}
		else {
			m_ppObjects[j]->Animate(fTimeElapsed, pCamera, false);	//false 일경우 플레이어 반경 100m 밖에 있는것
		}
	
	}
}


//////////////////////////////////////////////////////////////////
CSkyBoxShader::CSkyBoxShader()
{

}

CSkyBoxShader::~CSkyBoxShader()
{

}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSSkyBox", "ps_5_1", ppd3dShaderBlob));
}

void CSkyBoxShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}
