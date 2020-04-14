#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			21

struct INSTANCEDGAMEOBJECTINFO
{
	matrix gmtxGameObject;
	uint gintNum;
};

struct INSTANCEDGAMEUIINFO
{
	matrix gmtxGameUi;
	uint numUi;
};

struct INSTANCEDSKINEDOBJECTINFO
{
	float4x4 gpmtxInstancedBoneTransforms[SKINNED_ANIMATION_BONES];
};

cbuffer cbCameraInfo : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbShadowInfo : register(b3)
{
	matrix gmtxShadowView : packoffset(c0);
	matrix gmtxShadowProjection : packoffset(c4);
	matrix invViewProj : packoffset(c8);
	float3 gvShadowCameraPosition : packoffset(c12);
};

cbuffer cbObjectInfo : register(b5)
{
	matrix	gmtxWorld : packoffset(c0);
}
cbuffer cbObjectInfo : register(b5)
{
	INSTANCEDSKINEDOBJECTINFO	gmtxWorlds : packoffset(c0);
}

StructuredBuffer<INSTANCEDGAMEOBJECTINFO> gGameObjectInfos : register(t0);
StructuredBuffer<INSTANCEDSKINEDOBJECTINFO> gSkinedGameObjectInfos : register(t0);
StructuredBuffer<INSTANCEDGAMEUIINFO> gGameUiInfos : register(t0);

Texture2DArray gTexarray:register(t1);
TextureCube gCubeTexture : register(t1);
Texture2D gTexture : register(t1);
Texture2D gDetailedTexture : register(t2);
Texture2D gShadowMap : register(t3);

SamplerState gsamPointWrap  : register(s0);
SamplerState gsamPointClamp  : register(s1);
SamplerState gsamLinearWrap  : register(s2);
SamplerState gsamLinearClamp  : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp  : register(s5);
SamplerComparisonState gsamShadow : register(s6);

#include "Light.hlsl"

struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION1;
	float2 TexC : TEXCOORD;
	float3 normalW : NORMAL;
};
//---------------------스킨 매시----------------------------

cbuffer cbBoneOffsets : register(b7)
{
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b8)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

struct VS_SKINNED_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 TexC : TEXCOORD;
	int4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

struct VS_SKINNED_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 TexC : TEXCOORD;
};

struct VS_TEXT_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 TexC : TEXCOORD;
	float2 TexC1 : TEXCOORD1;
};

struct VS_TEXT_OUTPUT
{
	float4 position : SV_POSITION;    //최종 위치 -> 투영변환까지 적용
	float3 positionW : POSITION1;     //월드 위치
	float3 normalW : NORMAL;
	float2 TexC : TEXCOORD;
	float2 TexC1 : TEXCOORD1;
};

struct VS_UI_INPUT
{
	float3 position : POSITION;
	float2 TexC : TEXCOORD;
};

struct VS_UI_OUTPUT
{
	float4 position : SV_POSITION;    //최종 위치 -> 투영변환까지 적용
	float2 TexC : TEXCOORD;
	uint num : NUM;
};

VS_TEXT_OUTPUT VSTexting(VS_TEXT_INPUT input)
{
	VS_TEXT_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxWorld);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.TexC = input.TexC;

	return(output);
}
float4 PSTexting(VS_TEXT_OUTPUT input) : SV_TARGET
{

	float4 diffuseAlbedo = gTexture.Sample(gsamAnisotropicWrap,input.TexC);
	clip(diffuseAlbedo.a - 0.1f);
	float3 normalW = normalize(input.normalW);
	float4 color = (0.5*diffuseAlbedo); //+ (Lighting(input.positionW, normalW)*0.5);
	color.a = diffuseAlbedo.a;
	return color;
}

//-------------------텍스처o UI---------------------------------
VS_TEXT_OUTPUT VSUi(VS_TEXT_INPUT input)
{
	VS_TEXT_OUTPUT output;
	output.positionW = input.position;
	output.position = float4(input.position, 1.0f);
	output.normalW = input.normal;
	output.TexC = input.TexC;

	return(output);
}
float4 PSUi(VS_TEXT_OUTPUT input) : SV_TARGET
{
	float4 diffuseAlbedo = gTexture.Sample(gsamAnisotropicWrap,input.TexC);
	clip(diffuseAlbedo.a - 0.1f);

	float4 color = diffuseAlbedo;
	color.a = diffuseAlbedo.a;
	return color;
}

//----------------------텍스처o 테셀레이션o 지형-------------------------------------
struct HS_CONSTANT_OUTPUT
{
	float fTessEdge[3] : SV_TessFactor;
	float fTessInside[1]: SV_InsideTessFactor;
	float3 B11: POSITION1;
};
struct HS_OUTPUT
{
	float3 positionW : POSITION1;     //월드 위치
	float3 normalW : NORMAL;
	float2 TexC : TEXCOORD;
	float3 position[3] : POSITION2;
};
struct DS_TEXT_OUTPUT
{
	float4 position : SV_POSITION;    //최종 위치 -> 투영변환까지 적용
	float3 normalW : NORMAL;
	float2 TexC : TEXCOORD;
	float2 TexC1 : TEXCOORD1;
};


//-----------------텍스처o 인스턴싱o------------------------
struct VS_TEXTED_INSTANCING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 TexC : TEXCOORD;
};
struct VS_TEXTED_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 TexC : TEXCOORD;
	int n : NUM;
};

//------SkyBox
struct VS_SKYBOX_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};



float CalcShadowFactor(float4 shadowPosH)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	uint width, height, numMips;
	gShadowMap.GetDimensions(0, width, height, numMips);

	// Texel size.
	float dx = 1.0f / (float)width;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit / 9.0f;
}

//wire
struct VS_WIREFRAME_INPUT
{
	float3 position : POSITION;
};

struct VS_WIREFRAME_OUTPUT
{
	float4 position : SV_POSITION;
};