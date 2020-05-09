//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"
#include "Player.h"



CScene::CScene()
{
}

CScene::~CScene()
{
}

//#define _WITH_TERRAIN_PARTITION

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	//루트 시그니쳐를 생성하고 CScene내에서 쓰일 리소스들을 생성하고 씬내에서 관리 
	//즉 , Scene->BuildObjects에서 생성하고 있는 객체들에게 필요한 리소스들은 CScene의 루트시그니쳐에서 쉐이더에게 연결해주는것이다. 
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);			//루트 시그너쳐 생성 

	XMFLOAT3 xmf3Scale(8.0f, 2.0f, 8.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	// 스카이 박스 
	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

#ifdef _WITH_TERRAIN_PARTITION
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Image/ImageHeightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);
//	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);
#else
	// CHeightMapTerrain은 울퉁불퉁한 지형
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Image/HeightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color,0);
	// CPlaneTerrain은 평평한 지형
	m_pPlaneTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Image/HeightMapSand2.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color,1);
//	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);
#endif
	
	// 같은 쉐이더를 사용하여 그려야할 객체들은 한번에 set하고 한번에 draw하게 하기 위하여 , 즉 배치 처리를 위하여
	// Shader클래스를 나누어 씬에서 관리하고 있다. 
	// 즉 이 클래스들은 Scene->Render가 호출 되면 해당 쉐이더들이 
	// m_ppShaders -> Render를 호출하여 배치처리한다. 
	// 배치 처리를 하는 이유는 렌더링 과정에서 fps를 갉아 먹는 이유중에 하나가 set And Draw 이다. 
	// 물체 하나 그리려고 Set하고 Draw하고 또 하나 그리려고 SEt하고 Draw하는 과정이 반복되는것은 매우 비효율적.
	// 그렇기 때문에 같은 쉐이더와 같은 파이프라인 공정을 거치는 오브젝트들을 모아 한번만 set하고 모든 오브젝트들을 draw하는 방식을 취한다.
	m_nShaders = 2;
	m_ppShaders = new CShader*[m_nShaders];


	// 씬에서 쉐이더를 생성할때 아래 과정은 바뀌지 않는다.
	// 그 쉐이더가 어떤 정점쉐이더,픽셀쉐이더등을 사용할것인지를 
	// CreateShader에서 알려주고
	// 그 쉐이더 를 이용해서 그릴 오브젝트들을 BuildObject에서 생성해준다.


	//나무 빌보드 오브젝트를 그릴때 사용하는 
	CBillboardShader *pBillboardShader = new CBillboardShader();
	pBillboardShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pBillboardShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);
	m_ppShaders[0] = pBillboardShader;

	//풀 오브젝트를 그릴때 사용할 쉐이더 클래스
	CGrassBillboardShader *pGrassBillboardShader = new CGrassBillboardShader();
	pGrassBillboardShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pGrassBillboardShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pPlaneTerrain);
	m_ppShaders[1] = pGrassBillboardShader;


	//이 함수는 즉 씬 단위에서 넘겨주는 상수버퍼로는 (조명, 재질 , 안개, 등의) 정보가 있을것이다. 해당 예제에서는 따로 넘겨주지 않음
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	// 씬이 관리하고 있는 루트 시그니쳐 해제 // Com객체에서 불러오는 인터페이스이므로 delete 하면 안된다. 참조 카운트만 -1해준다. 
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	//씬이 관리하고 있는 쉐이더 클래스들 해제
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	ReleaseShaderVariables();

	//씬이 관리하는 즉, 쉐이더 클래스 말고 오브젝트 자체에서 쉐이더 정의 하고 있는 객체들을 delete
	if (m_pTerrain) delete m_pTerrain;
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	//씬이 관리하는 루트 시그니쳐 
	// 즉 pScene->Render함수 에는 가장 먼저 SetGraphicsRootSignature가 호출되어야 한다.
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	//D3D12_DESCRIPTOR_RANGE는 일종의 DescriptorTable에 들어갈 Descriptor들을 설정한것이라 할 수 있다.
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[9];

	// DescriptorTable의 첫번째 원소는 CBV (상수 버퍼 뷰) 
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 2; //GameObject
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//두번째 원소는 SRV로 (쉐이더 리소스 뷰)
	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 0; //t0: gtxtTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;						//베이스 텍스쳐
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 1; //t1: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;						//디테일 텍스쳐
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 2; //t2: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 4;  //t4 : gtxtGrassBillboardTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 5;	  //t5 : gtxtSandBaseTexture;
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;						//빌보드 텍스쳐
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 3; //t3: gtxtBillboardTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;						//스카이박스 텍스쳐
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 6; //t6: gtxtSkyBoxTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;						//스카이박스 텍스쳐
	pd3dDescriptorRanges[8].NumDescriptors = 1;
	pd3dDescriptorRanges[8].BaseShaderRegister = 8; //t8: gtxtFlowerBillboardTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	D3D12_ROOT_PARAMETER pd3dRootParameters[12];

	//서술자

	// 파라 미터 타입으로 3가지가 있다. 
	// 1.D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE 
	// 2.D3D12_ROOT_PARAMETER_TYPE_CBV (D3D12_ROOT_PARAMETER_TYPE_SRV) (D3D12_ROOT_PARAMETER_TYPE_UAV)
	// 3.D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANT

	// 1번 유형은 디스크립터 테이블로써 테이블은 위의 DiscriptorRange에 정의된 테이블 번호에 서술자를 연결해준다.(쉽게 생각하면 CBV가 리소스를 가리키는 포인터이고 그 포인터를 담고있는 배열이라 생각하면 좋을듯) 
	// 2 번 유형은 리소스를 가리키는 서술자로써 리소스를 가리키는 포인터라고 생각해도 좋을것 같다.
	// 3번 유형은 루트 상수라고 하는데 포인터가 아닌 실제 그 값을 전달해주는것이라고 생각해도 좋다. (실제 3D 1 수업에서는 각 행렬을 루트상수로만 전달했음)
	// 3번이 가장 빠르지만 루트 시그니쳐는 32BIT *64 개가 한계인데 루트 상수 한개가 32BIT 이고 행렬을 넘겨준다고 하면 32BIT * 16 개 이므로 루트시그니쳐에서 차지하는 부분이 크기때문에
	// 실제 게임에서는 사용할 수 없음. 
	// 2번 루트 서술자는 64BIT로 MAX : 32개 까지 루트 시그니쳐에 연결 가능하다
	// 1번 루트 디스크립터 테이블은 32BIT로 64개까지 연결 가능하다. 씬의 텍스쳐가 여러개 필요할경우 디스크립터 테이블을 사용하는 것이 좋다.
	// 실제로 텍스쳐는 테이블로만 연결 가능하다. 

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	//서술자 테이블

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			//테이블의 첫번째 원소는 cbv
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			//t0: gtxtTexture
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						//텍스쳐는 일반적으로 픽셀쉐이더에서 이용됨

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;				//t1: gtxtTerrainBaseTexture
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2];
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;				//t2: gtxtTerrainDetailTexture
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3];
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;				//t3: gtxtSandboardTexture
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[5];
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;				//t4 : gxtxGrassBillboardTexture
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4];			
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;				//t5 : gxtxBillboardBaseTexture
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[6];
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;				//t6 : gxtxSkyBoxTexture
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[7];
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[10].Descriptor.ShaderRegister = 7;											//t7 : gGameObjectInfos
	pd3dRootParameters[10].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;				//t8 : gxtxFlowerBillboardTexture
	pd3dRootParameters[11].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[11].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[8];
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;





	//static sampler하면 단점 딱 하나 -> ordercolor를 내맘대로 지정 못함
	// 샘플러 - 서버는 알 필요 없을듯

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;									//정적 샘플러 
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;			//샘플링할때 사용할 필터링 방법 (선형 필터링)
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;			//0~1을 벗어난 u-좌표의 해결 방법.
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;			//반복
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;										//Direct3D가 계산한 LOC(밉맵) 레벨에 이 값을 더한 밉맵 레벨에서 샘플링.
	d3dSamplerDesc.MaxAnisotropy = 1;									//필터가_ 비등방일때 사용되는 레벨값(1~16)
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;		//샘플링한 값을 비교하기 위한 함수
	d3dSamplerDesc.MinLOD = 0;											//최소 밉맵 레벨, 밉맵 레벨 0이 가장 크고 세밀한 밉맵
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;							//최대 밉맵 레벨, MinLOD보다 크거나 같아야함.
	d3dSamplerDesc.ShaderRegister = 0;									//쉐이더 번호 (s0)
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;				//루트 파라미터를연결
	d3dRootSignatureDesc.NumStaticSamplers = 1;							//정적 샘플러 개수 1개
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;				//정적 샘플러는 따로 연결
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);			//ID3D12Device::CreateRootSignature에 전달될수 있는 루트시그너쳐 버전 1.0을 직렬화시킨다.
																																	//첫 번째 인자:D3D12_ROOT_SIGNATURE_DESC 구조체의 포인터로써 루트 시그니처를 설명한다.
																																	//세	 번째 인자 : 직렬화된 루트시그너쳐의 접근할수 있게 해주는 ID3DBlob 인터페이스를 가리키는 포인터를 받는 메모리 블록
	// 이 부분에서 루트 시그니쳐 생성함
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	//두번째 인자 : 직렬화된 루트 시그너쳐에 대한 데이터 포인터
	//세번째 인자 : 루트 시그너쳐의 크기(바이트 수)
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CScene::ReleaseShaderVariables()
{
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	

	// 씬이 관리하는 쉐이더 클래스들에게 쉐이더 클래스가 관리하는 오브젝트들을
	// 갱신하게 함.
	// 즉, cpu에서 처리해야할 씬안의 오브젝트의 이동, 회전, 충돌 검사, 등을 실시함
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(fTimeElapsed, m_pPlayer->GetCamera(), m_pPlayer);
	}
	
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{

	// 씬의 렌더에서 순서가 중요하다 . 
	// SetGraphicsRootSignature를 호출함으로써 앞으로 사용할 리소스들이 연결된
	// 루트 시그니쳐를 set해준다.
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	//뷰포트 설정과 씨져렉트는 컴퓨터 화면상의 그려질 위치를 정해주는건데
	// 서버는 알 필요는 없어보임.
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	
	//씬이 갖고 있는 Camera 객체를 이용해 그 카메라로 부터의 뷰행렬과 프로젝션 행렬을
	// Shaders.hlsl 상수버퍼 1번(b1)에 넘겨준다.
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	/////////////////////////////////////////////////////////////////////////////
	// 이 세 객체들은 객체 자신들이 쉐이더 클래스에 대한 포인터를 갖고 셋해주기 때문에
	// 객체를 직접 호출해서 렌더해준다. 
	// 이 이유는 이 객체들은 씬의 단 한개씩만 존재하기때문에 배치처리가 필요없기 때문
	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
	if(m_pPlaneTerrain) m_pPlaneTerrain->Render(pd3dCommandList,pCamera);
//////////////////////////////////////////////////////////////////////////////////
	

	// 배치처리가 필요한 객체들은 이런식으로 쉐이더 클래스를 따로두어 
	// 한꺼번에 그리게 한다. (즉, set And Draw를 한 쉐이더 클래스당 한번만 실시함)
	// (위의 내용은 인스턴싱이랑은 좀 다른 개념이므로 헷갈리면 안됨)
	// 서버는 자세히 알 필요는 없지만 어느정도 배치 처리에대한 개념은 알아두면 좋을듯
	
	
	
	// 다이렉트 x12에 렌더링 파이프라인을 실행하기에 앞서 cpu에서 준비해야할 과정이 있다.
	// CShader::OnPrepareRender()함수가 그 역할을 하는것이라 볼수있는데
	// 그 함수에 들어가면 그리기에 앞서 해야 할 작업들을 해주는 함수라고 생각할수 있겟다.
	// 이함수는 direct 함수가 아니므로 헷갈리지 말것. 
	// 즉 이러한 함수가 있다는 것은 즉 이 과정은 어떠한 CShader의 자식 클래스도 
	// 자신의 Render전에 이러한 과정이 필요하다는 의미이다. 
	// CShader::OnPrepareRender() 함수를 가보면 
	
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	}
}

