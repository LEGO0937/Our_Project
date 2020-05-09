
cbuffer cbPlayerInfo : register(b0)							//Player (서술자)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)							//Camera (서술자)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
};

cbuffer cbGameObjectInfo : register(b2)						//GameObject (서술자 테이블)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnObjectID	: packoffset(c4);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTexture : register(t0);
Texture2D gtxtBillboardTexture : register(t3);
Texture2D gtxtGrassBillboardTexture : register(t4);
Texture2D gtxtSkyBoxTexture : register(t6);
SamplerState gSamplerState : register(s0);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtBillboardTexture.Sample(gSamplerState, input.uv);

	if (cColor.a < 0.3f)
	{
		discard;
	}
	return(cColor);
}


float4 PSGrassTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	
	float4 cColor = gtxtGrassBillboardTexture.Sample(gSamplerState,input.uv);
	
	

	if (cColor.a <0.4f)
	{
		discard;
	}
	return (cColor);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

float4 PSSkyBox(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyBoxTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTerrainBaseTexture : register(t1);
Texture2D gtxtTerrainDetailTexture : register(t2);
Texture2D gtxtSandBaseTexture : register(t5);


struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};


VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;
	
	output.position = mul(mul(mul(float4(input.position,1.0f),gmtxGameObject), gmtxView), gmtxProjection);

	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{


	float4 cBaseTexColor;
	if (gnObjectID == 0) {
		cBaseTexColor=gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);
	}
	else if (gnObjectID == 1)
	{
		cBaseTexColor = gtxtSandBaseTexture.Sample(gSamplerState, input.uv0);

	}
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gSamplerState, input.uv1);
	float4 cColor = /*input.color **/ cBaseTexColor + cDetailTexColor;	//기존 albedo * 텍스쳐 원본 으로 초록색 지형이나 빨간색 지형을 만들 수 있다.
	//float4 cColor=saturate(input.color * 0.5f + (cBaseTexColor + cDetailTexColor)*0.5f);

	return(cColor);

}
///////////////////////////////////////////////////////////////////////////

Texture2D gtxtFlowerBillboardTexture : register(t8);

struct INSTANCEDGAMEOBJECTINFO
{
	matrix	m_mtxGameObject :WORLDMATRIX;
	//float2	uv :INSTANCETEXCOORD;
};
StructuredBuffer<INSTANCEDGAMEOBJECTINFO> gGameObjectInfos : register(t7);
struct VS_INSTANCING_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	//float4x4 mtxTransform : WORLDMATRIX;
	//float4 instanceColor : INSTANCECOLOR;

};
struct VS_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_INSTANCING_OUTPUT VSInstancing(VS_INSTANCING_INPUT input, uint nInstanceID :SV_InstanceID)
{
	VS_INSTANCING_OUTPUT output;


	output.position = mul(mul(mul(float4(input.position, 1.0f),
		gGameObjectInfos[nInstanceID].m_mtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;
	return(output);
}

float4 PSInstancing(VS_INSTANCING_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtFlowerBillboardTexture.Sample(gSamplerState, input.uv);

	if (cColor.a < 0.3f) {
		discard;
	}

	return(cColor);
}


