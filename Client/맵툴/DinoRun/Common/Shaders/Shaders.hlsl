cbuffer cbGameObjectInfo : register(b1)
{
	matrix	gmtxT : packoffset(c0);
	uint degree : packoffset(c4);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix	gmtxWorld : packoffset(c0);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCameraInfo : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	float3 gvCameraPosition : packoffset(c8);
};
struct INSTANCEDGAMEOBJECTINFO
{
	matrix gmtxGameObject;
};
cbuffer ObjectInfo : register(b5)
{
	matrix	World : packoffset(c0);
}

//cbuffer cbShadow : register(b5)
//{
//	matrix View;
//	matrix Proj;
//	float3 lightPosition;
//	float pad0;
//	matrix Transform;
//	float farZ;
//	float nearZ;
//	float pad1;
//	float pad2;
//};


StructuredBuffer<INSTANCEDGAMEOBJECTINFO> gGameObjectInfos : register(t0);

Texture2D gDiffuseMap : register(t1);
Texture2D gTexture : register(t1);
Texture2D gDetailedTexture : register(t2);

SamplerState gsamPointWrap  : register(s0);
SamplerState gsamPointClamp  : register(s1);
SamplerState gsamLinearWrap  : register(s2);
SamplerState gsamLinearClamp  : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp  : register(s5);
SamplerComparisonState gsamShadow : register(s6);
//인스턴싱 데이터를 위한 구조체이다. 



#include "Light.hlsl"


//정점 조명을 사용

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
#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			128

cbuffer cbBoneOffsets : register(b7)
{
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b8)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};
struct VS_SKINNED_WIREFRAME_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 TexC : TEXCOORD;
	int4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

struct VS_SKINNED_WIREFRAME_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 TexC : TEXCOORD;
};

VS_SKINNED_WIREFRAME_OUTPUT VSSkinnedAnimationWireFrame(VS_SKINNED_WIREFRAME_INPUT input)
{
	VS_SKINNED_WIREFRAME_OUTPUT output;

	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	float3 normalW = float3(0.0f, 0.0f, 0.0f);
	matrix mtxVertexToBoneWorld;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld);

		normalW += input.weights[i] * mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	}

	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	output.normal = positionW;
	output.TexC = input.TexC;
	return(output);
}

float4 PSSkinnedAnimationWireFrame(VS_SKINNED_WIREFRAME_OUTPUT input) : SV_TARGET
{
	float4 diffuseAlbedo = gTexture.Sample(gsamAnisotropicWrap,input.TexC);

	return(diffuseAlbedo);
}
//----------------------텍스처x 인스턴스x-----------------------------------------------------------------
VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxWorld);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);

	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{

	float3 normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, normalW);
	return(color);
}

//-------------------텍스처o 인스턴스x------------------------

struct VS_TEXT_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 TexC : TEXCOORD;
	float2 TexC1 : TEXCOORD1;
};
//정점 쉐이더의 출력 정점 구조
struct VS_TEXT_OUTPUT
{
	float4 position : SV_POSITION;    //최종 위치 -> 투영변환까지 적용
	float3 positionW : POSITION1;     //월드 위치
	float3 normalW : NORMAL;
	float2 TexC : TEXCOORD;
	float2 TexC1 : TEXCOORD1;
};

VS_TEXT_OUTPUT VSTexting(VS_TEXT_INPUT input)
{
	VS_TEXT_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), World);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)World);
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
//----------------------텍스처o 파도-----------------------------------------
VS_TEXT_OUTPUT VSSurf(VS_TEXT_INPUT input)
{
	float3 pos = input.position;
	pos.y = 20 * sin(radians(pos.y + degree));
	input.TexC = (float2)mul(float4(input.TexC, 0.0f, 1.0f), gmtxT);
	VS_TEXT_OUTPUT output;
	output.positionW = (float3)mul(float4(pos, 1.0f), gmtxWorld);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);

	output.TexC = input.TexC;
	output.TexC1 = input.TexC;
	return(output);
}

float4 PSSurf(VS_TEXT_OUTPUT input) : SV_TARGET
{
	float4 diffuseAlbedo = gTexture.Sample(gsamAnisotropicWrap,input.TexC);
	clip(diffuseAlbedo.a - 0.1f);
	float3 normalW = normalize(input.normalW);
	float4 color = (0.5*diffuseAlbedo) + (Lighting(input.positionW, normalW)*0.5);
	//-20~20
	color.a = 0.7;//1.0 - (float(input.positionW.y *0.025 )+0.5);
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
//--------------------텍스처o 큐브맵---------------------------------------
TextureCube gCubeMap: register(t1);
VS_TEXT_OUTPUT VSCube(VS_TEXT_INPUT input)
{
	VS_TEXT_OUTPUT output;
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y,
		gvCameraPosition.z);
	output.positionW = input.position;
	float3 posW = mul(float4(input.position, 1.0f), World).xyz;
	float3 normalW = mul(input.normal, (float3x3) World);
	posW.xyz += vCameraPosition;


	output.position = mul(mul(float4(posW, 1.0f), gmtxView), gmtxProjection);
	output.TexC = input.TexC;
	output.normalW = normalW;

	return(output);
}
//픽셀 쉐이더 함수
float4 PSCube(VS_TEXT_OUTPUT input) : SV_TARGET
{

	return gCubeMap.Sample(gsamLinearWrap,input.positionW);
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

float wij(VS_TEXT_OUTPUT i, VS_TEXT_OUTPUT j)
{
	return dot((j.positionW - i.positionW), i.normalW);
}

VS_TEXT_OUTPUT VSTer(VS_TEXT_INPUT input)
{
	VS_TEXT_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), World);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)World);
	output.TexC = input.TexC;
	output.TexC1 = input.TexC1*50.f;
	return(output);
}
HS_CONSTANT_OUTPUT HSConstant(InputPatch<VS_TEXT_OUTPUT, 3> input, OutputPatch<HS_OUTPUT, 3> I, uint patchID : SV_PrimitiveID)
{

	float3 vCenter3 = (input[0].positionW + input[1].positionW) / 2.0f;
	float3 vCenter1 = (input[1].positionW + input[2].positionW) / 2.0f;
	float3 vCenter2 = (input[0].positionW + input[2].positionW) / 2.0f;

	float fDistanceToCamera1 = distance(vCenter1, gvCameraPosition);
	float fDistanceToCamera2 = distance(vCenter2, gvCameraPosition);
	float fDistanceToCamera3 = distance(vCenter3, gvCameraPosition);

	float fMin = 10.0f, fMax = 800.0f;
	float m = 1.0f;

	HS_CONSTANT_OUTPUT output;

	float fTessFactor = 12.0f*saturate((fMax - fDistanceToCamera1) / (fMax - fMin));
	if (fTessFactor < 1)
		fTessFactor = 1.0f;
	output.fTessEdge[0] = fTessFactor;
	m = max(fTessFactor, m);
	fTessFactor = 12.0f*saturate((fMax - fDistanceToCamera2) / (fMax - fMin));
	if (fTessFactor < 1)
		fTessFactor = 1.0f;
	output.fTessEdge[1] = fTessFactor;
	m = max(fTessFactor, m);
	fTessFactor = 12.0f*saturate((fMax - fDistanceToCamera3) / (fMax - fMin));
	if (fTessFactor < 1)
		fTessFactor = 1.0f;
	output.fTessEdge[2] = fTessFactor;
	m = max(fTessFactor, m);

	output.fTessInside[0] = m;



	float3 B300 = I[0].position[0],
		B210 = I[0].position[1],
		B120 = I[0].position[2],
		B030 = I[1].position[0],
		B021 = I[1].position[1],
		B012 = I[1].position[2],
		B003 = I[2].position[0],
		B102 = I[2].position[1],
		B201 = I[2].position[2];

	float3 E = (B210 + B120 + B021 + B012 + B102 + B201) / 6.0f;
	float3 V = (B003 + B030 + B300) / 3.0f;
	output.B11 = E + ((E - V) / 2.0f);
	return output;

}


[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
[maxtessfactor(64.0f)]
HS_OUTPUT HS(InputPatch<VS_TEXT_OUTPUT, 3> input, uint i: SV_OutputControlPointID)
{
	//-------012 345 678
	//  0-1 : 0,1,2  1-2:3,4,5   2-0:6,7,8  cen:9
	//맨위부터 시계 방향으로 030(0) 021 012 003(1) 102 201 300(2) 210 120  가운데: 111

	HS_OUTPUT output;
	const int next = (i + 1) % 3;

	float3 ij = wij(input[i], input[next]);
	float3 ji = wij(input[next], input[i]);
	output.TexC = input[i].TexC;
	output.normalW = input[i].normalW;
	output.positionW = input[i].positionW;

	output.position[0] = input[i].positionW;
	output.position[1] = (2 * input[i].positionW +
		input[next].positionW - ij * input[i].normalW) / 3;
	output.position[2] = (2 * input[next].positionW +
		input[i].positionW - ji * input[next].normalW) / 3;
	return output;
}

[domain("tri")]
VS_TEXT_OUTPUT DS(HS_CONSTANT_OUTPUT input, float3 uv: SV_DomainLocation, OutputPatch<HS_OUTPUT, 3> tri)
{
	VS_TEXT_OUTPUT output;

	float u = uv.x;
	float v = uv.y;
	float w = uv.z;

	float uu = u * u;
	float vv = v * v;
	float ww = w * w;
	float uu3 = uu * 3.0f;
	float vv3 = vv * 3.0f;
	float ww3 = ww * 3.0f;

	float3 Position = tri[0].position[0] * uu*u +
		tri[1].position[0] * vv*v +
		tri[2].position[0] * ww*w +
		tri[0].position[1] * uu3*v +
		tri[0].position[2] * vv3*u +
		tri[1].position[1] * vv3*w +
		tri[1].position[2] * ww3*v +
		tri[2].position[1] * ww3*u +
		tri[2].position[2] * uu3*w +
		input.B11*6.0f*w*u*v;
	output.positionW = tri[0].positionW*u +
		tri[1].positionW*v +
		tri[2].positionW*w;
	output.position = mul(mul(float4(Position, 1.0f), gmtxView), gmtxProjection);

	output.normalW = tri[0].normalW*u +
		tri[1].normalW*v +
		tri[2].normalW*w;
	output.normalW = normalize(output.normalW);

	output.TexC = tri[0].TexC*u +
		tri[1].TexC*v +
		tri[2].TexC*w;
	output.TexC1 = output.TexC*50.0f;
	return output;

}
float4 PSTer(VS_TEXT_OUTPUT input) : SV_TARGET
{
	float4 baseAlbedo = gTexture.Sample(gsamLinearWrap,input.TexC);
	float4 detailedAlbedo = gDetailedTexture.Sample(gsamLinearWrap, input.TexC1);

	clip(baseAlbedo.a - 0.1f);

	float3 normalW = normalize(input.normalW);
	float4 color = saturate(0.5*(detailedAlbedo + baseAlbedo) + Lighting(input.positionW, normalW)*0.5);
	color.a = baseAlbedo.a;
	return color;
}
//-----------------텍스처x 인스턴싱o--------------------------

struct VS_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	int n : NUM;
};


VS_INSTANCING_OUTPUT VSInstancing(VS_LIGHTING_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_INSTANCING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gGameObjectInfos[nInstanceID].gmtxGameObject);

	output.n = nInstanceID;
	return(output);
}

float4 PSInstancing(VS_INSTANCING_OUTPUT input) : SV_TARGET
{

	float3 normalW = normalize(input.normalW);

	float4 color = (Lighting(input.positionW, normalW));
	return(color);
}
//-----------------텍스처o 인스턴싱o------------------------
struct VS_textedINSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION1;
	float3 normalW : NORMAL;
	float2 TexC : TEXCOORD;
	int n : NUM;
};

VS_textedINSTANCING_OUTPUT VStextedInstancing(VS_TEXT_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_textedINSTANCING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.TexC = input.TexC;
	output.n = nInstanceID;
	return(output);
}
Texture2DArray gTexarray:register(t1);
float4 PStextedInstancing(VS_textedINSTANCING_OUTPUT input) : SV_TARGET
{
	float3 normalW = normalize(input.normalW);
	float3 uvw = float3(input.TexC, input.n);
	float4 color = gTexarray.Sample(gsamAnisotropicWrap,uvw);
	clip(color.a - 0.1);
	color = color * 0.5 + (Lighting(input.positionW, normalW)*0.5);
	return(color);
}
//----------------------텍스처o 인스턴싱o 기하o----------------------------------------

VS_textedINSTANCING_OUTPUT VSgtextedInstancing(VS_TEXT_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_textedINSTANCING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.TexC = input.TexC;
	output.n = nInstanceID;

	return(output);
}
[maxvertexcount(4)]
void GS(point VS_textedINSTANCING_OUTPUT input[1], uint primID :SV_PrimitiveID, inout TriangleStream<VS_textedINSTANCING_OUTPUT> outStream)
{
	float3 vUp = float3(0, 1, 0);
	float3 vLook = float3(gvCameraPosition.xyz - input[0].positionW.xyz);
	vLook.y = 0;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fWidth = 150;
	float fHeight = 150;
	float4 pVertices[4];
	pVertices[0] = float4(input[0].positionW + fWidth * vRight - fHeight * vUp, 1.0f);
	pVertices[1] = float4(input[0].positionW + fWidth * vRight + fHeight * vUp, 1.0f);
	pVertices[2] = float4(input[0].positionW - fWidth * vRight - fHeight * vUp, 1.0f);
	pVertices[3] = float4(input[0].positionW - fWidth * vRight + fHeight * vUp, 1.0f);
	float2 pUVs[4] = { float2(1.0f,1.0f),float2(1.0f,0.0f) ,float2(0.0f,1.0f) ,float2(0.0f,0.0f) };

	VS_textedINSTANCING_OUTPUT output;

	for (int i = 0; i < 4; ++i)
	{
		output.positionW = pVertices[i].xyz;
		output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
		output.normalW = vLook;
		output.TexC = pUVs[i];
		output.n = input[0].n;
		outStream.Append(output);
	}

}
//------SkyBox
struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), World), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return(output);
}

TextureCube gtxtSkyCubeTexture : register(t1);

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gsamLinearWrap, input.positionL);

	return(cColor);
}
