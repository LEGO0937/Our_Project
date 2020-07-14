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

	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	output.normal = positionW;
	output.TexC = input.TexC;
	return(output);
}

float4 PSSkinnedAnimation(VS_SKINNED_OUTPUT input) : SV_TARGET
{
	float4 diffuseAlbedo = gTexture.Sample(gsamAnisotropicWrap,input.TexC);

	return(diffuseAlbedo);
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

	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	output.normal = positionW;
	output.TexC = input.TexC;
	return(output);
}