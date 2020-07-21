#pragma once
#include "../targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <Mmsystem.h>

// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
#include <string>
#include <wrl.h> 
#include <shellapi.h>
#include <d3d12.h> 
#include <dxgi1_4.h> 
#include <D3Dcompiler.h> 
#include <DirectXMath.h> 
#include <DirectXPackedVector.h> 
#include <DirectXColors.h> 
#include <DirectXCollision.h>

#include <random>
#include <chrono>
#include <vector>
#include <deque>
#include <array>
#include <unordered_map>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>

#include <fmod.hpp>
using namespace FMOD;

#include "../protocol.h"
#include "../Common/TextureLoad/DDSTextureLoader12.h"
#include "../Global/Global.h"
using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

using namespace std;
#pragma comment(lib, "d3d12.lib") 

#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "Ws2_32.lib")
#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 700
//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

#define _WITH_LEFT_HAND_COORDINATES 왼손좌표계 y or n
//#define _WITH_BOUND_BOX   바운딩 박스 랜더링

#define MAX_LIGHTS 8
#define MAX_MATERIALS 8
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
#define DIRECTIONAL_LIGHT 3

#define SKINNED_ANIMATION_BONES		23
const UINT N_SWAPCHAINBUFFERS = 2; //스왑 체인의 후면 버퍼의 개수
const UINT N_RENDERTARGETBUFFERS = 1; //스왑 체인의 후면 버퍼의 개수

typedef struct Point2D 
{
	float x;
	float y;
}Point2D;

struct MessageStruct
{
	string msgName;
	string shaderName;
	string objectName = "None";
	int integerValue;
	XMFLOAT4X4 departMat;
	XMFLOAT4X4 arriveMat;

	MessageStruct() {}
	MessageStruct(const MessageStruct& msg)
	{
		msgName = msg.msgName;
		shaderName = msg.shaderName;
		integerValue = msg.integerValue;
		departMat = msg.departMat;
		arriveMat = msg.arriveMat;
		objectName = msg.objectName;
	}
};

extern ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE
	d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates =
	D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource **ppd3dUploadBuffer =
	NULL, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
extern ID3D12Resource *CreateTextureResourceFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, ID3D12Resource **ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates);
ComPtr<ID3D12Resource> CreateTexture2DResource(ID3D12Device *pd3dDevice, UINT nWidth, UINT nHeight,	DXGI_FORMAT format,	D3D12_RESOURCE_FLAGS resourceFlags,
	D3D12_RESOURCE_STATES resourceStates,	D3D12_CLEAR_VALUE * pClearValue);

extern UINT	gnCbvSrvDescriptorIncrementSize;
//3차원 벡터의 연산
namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}
	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize =
		true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) *
				fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}
	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) +
			XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2)
			* fScalar));
		return(xmf3Result);
	}
	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) -
			XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline XMFLOAT3 Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) -
			XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline XMFLOAT3 DivProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize =
		true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) /
				fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) / fScalar);
		return(xmf3Result);
	}
	

	inline float DotProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1),
			XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}
	inline XMFLOAT3 CrossProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, bool
		bNormalize)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result,
				XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1),
					XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1),
				XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}
	inline XMFLOAT3 Normalize(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}
	inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}
	inline float Length(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}
	inline float Length(const XMFLOAT3& v1, const XMFLOAT3& v2)
	{		
		return ((v2.x - v1.x)*(v2.x - v1.x) + (v2.y - v1.y)*(v2.y - v1.y) 
			+ (v2.z - v1.z)*(v2.z - v1.z));	
	}
	inline float Angle(const XMVECTOR& xmvVector1, const XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
	}
	inline float Angle(const XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	}
	inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	}
	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector),
			xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector),
			xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}
}
//4차원 벡터의 연산
namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) +
			XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
	inline XMFLOAT4 Add(const XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) +
			XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}

}
//행렬의 연산
namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Zero()
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixSet(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
		return(xmf4x4Result);
	}
	inline XMFLOAT4X4 Scale(const XMFLOAT4X4& xmf4x4Matrix, float fScale)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmf4x4Matrix) * fScale);
		/*
				XMVECTOR S, R, T;
				XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&xmf4x4Matrix));
				S = XMVectorScale(S, fScale);
				T = XMVectorScale(T, fScale);
				R = XMVectorScale(R, fScale);
				//R = XMQuaternionMultiply(R, XMVectorSet(0, 0, 0, fScale));
				XMStoreFloat4x4(&xmf4x4Result, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
		*/
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Add(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) + XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmf4x4Result);
	}
	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) *
			XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1,const XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	//XMMatrixRotationQuaternion
	inline XMFLOAT4X4 Multiply(const XMMATRIX& xmmtxMatrix1,const XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 MultiplyQuarternion(const XMFLOAT4& xmf4Quarternin, const XMFLOAT4X4& xmmtx4x4Matrix1)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMMATRIX quarternion = XMMatrixRotationQuaternion(XMLoadFloat4(&xmf4Quarternin));
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * quarternion);
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL,
			XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result,
			XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ,
		float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio,
			NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 PerspectiveFovRH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixPerspectiveFovRH(FovAngleY, AspectRatio, NearZ, FarZ));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 OrthoLH(float ViewWidth, float ViewHeight, float NearZ,
		float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixOrthographicLH(ViewWidth, ViewHeight,
			NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 OrthoRH(float ViewWidth, float ViewHeight, float NearZ,
		float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixOrthographicRH(ViewWidth, ViewHeight,
			NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition,
		XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition),
			XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookAtLH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookAtPosition,
		const XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition),
			XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookAtRH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookAtPosition, const XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixLookAtRH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmf4x4Result);
	}
}


vector<string> split(string target, string delimiter);



/*정점의 색상을 무작위로(Random) 설정하기 위해 사용한다. 각 정점의 색상은 난수(Random Number)를 생성하여
지정한다.*/
#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

#define EPSILON					1.0e-10f

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline bool IsZero(float fValue, float fEpsilon) { return((fabsf(fValue) < fEpsilon)); }
inline bool IsEqual(float fA, float fB, float fEpsilon) { return(::IsZero(fA - fB, fEpsilon)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float *pfS, float *pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }



wchar_t* ConvertCHARtoWCHAR(const char* str);
Point2D ScreenToProj(int width, int height, POINT& point);

// Pipeline's Base Setting Function
D3D12_RASTERIZER_DESC CreateRasterizerState();
D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
D3D12_BLEND_DESC CreateBlendState();
D3D12_BLEND_DESC CreateAlphaBlendState();
D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);
D3D12_INPUT_LAYOUT_DESC CreateUiInputLayout();
D3D12_INPUT_LAYOUT_DESC CreateTextureInputLayout();

D3D12_RASTERIZER_DESC CreateShadowRasterizerState();
D3D12_DEPTH_STENCIL_DESC CreateShadowDepthStencilState();

//Base Pipelines
void CreatePsoSkinMesh(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoSkinedInstancing(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoTextedInstancing(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoBillBoardInstancing(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoTerrain(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoCube(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoUi(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoUiNumber(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoUiGuage(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoParticle(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoMinimap(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoPostEffect(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);


//Shadow Pipelines
void CreatePsoShadowSkinMesh(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoShadowSkinedInstancing(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoShadowTextedInstancing(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoShadowBillBoardInstancing(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoShadowTerrain(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);

//Velocity Pipelines
void CreatePsoVelocitySkinMesh(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoVelocitySkinedInstancing(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoVelocityTextedInstancing(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoVelocityBillBoardInstancing(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoVelocityTerrain(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoVelocityCubeMap(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);

//WireFrame pipelines
void CreatePsoWire(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoWireInstance(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);

//Font PipeLine
void CreatePsoFont(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);

//Compute PipeLine
void CreatePsoMotionBlur(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoHorzBlur(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoVertBlur(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
void CreatePsoParticleCs(ID3D12Device *pd3dDevice, ID3D12RootSignature* m_pd3dRootSignature, ID3D12PipelineState** m_ppd3dPipelineStates, int idx);
