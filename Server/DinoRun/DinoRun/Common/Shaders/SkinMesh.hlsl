#include "Shaders.hlsl"

VS_SKINNED_OUTPUT VSSkinnedAnimation(VS_SKINNED_INPUT input)
{
	VS_SKINNED_OUTPUT output;

	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	float3 normalW = float3(0.0f, 0.0f, 0.0f);
	matrix mtxVertexToBoneWorld;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;

		normalW += input.weights[i] * mul(input.normal,(float3x3)mtxVertexToBoneWorld).xyz;
	}

	output.positionW = positionW;
	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	output.normal = normalW;
	output.TexC = input.TexC;
	return(output);
}

float4 PSSkinnedAnimation(VS_SKINNED_OUTPUT input) : SV_TARGET
{
	/*
	float4 ShadowPosH = mul(float4(input.positionW, 1.0f), invViewProj);
	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	shadowFactor[0] = CalcShadowFactor(ShadowPosH);

	float4 diffuseAlbedo = gTexture.Sample(gsamAnisotropicWrap,input.TexC);
	float3 normalW = normalize(input.normal);

	diffuseAlbedo = diffuseAlbedo * 0.5 + (Lighting(input.positionW, normalW)*0.5);

	return(diffuseAlbedo);
	*/
	
	
	float4 ShadowPosH = mul(float4(input.positionW, 1.0f), invViewProj);
	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	shadowFactor[0] = CalcShadowFactor(ShadowPosH);

	float4 baseAlbedo = gTexture.Sample(gsamLinearWrap, input.TexC);

	clip(baseAlbedo.a - 0.1f);

	float3 normalW = normalize(input.normal);
	float4 color = saturate(0.45*(baseAlbedo) + sLighting(input.positionW, normalW, shadowFactor)*0.6);
	
	return color;
	
	
}

//-----------

VS_SKINNED_OUTPUT VSSkinnedInstancingAnimation(VS_SKINNED_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_SKINNED_OUTPUT output;

	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	float3 normalW = float3(0.0f, 0.0f, 0.0f);
	matrix mtxVertexToBoneWorld;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gSkinedGameObjectInfos[nInstanceID].gpmtxInstancedBoneTransforms[input.indices[i]]);
		positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;

		normalW += input.weights[i] * mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	}
	output.positionW = positionW;
	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	output.normal = normalW;
	output.TexC = input.TexC;
	return(output);
}