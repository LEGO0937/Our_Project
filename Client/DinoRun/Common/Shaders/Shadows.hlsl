#include "Shaders.hlsl"



VS_SKINNED_OUTPUT VSShadowSkinnedAnimation(VS_SKINNED_INPUT input)
{
	VS_SKINNED_OUTPUT output;

	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	float3 normalW = float3(0.0f, 0.0f, 0.0f);
	matrix mtxVertexToBoneWorld;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		positionW += (input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld)).xyz;

		normalW += input.weights[i] * mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	}

	output.position = mul(mul(float4(positionW, 1.0f), gmtxShadowView), gmtxShadowProjection);
	output.normal = positionW;
	output.TexC = input.TexC;
	return(output);
}

VS_SKINNED_OUTPUT VSShadowSkinnedInstancingAnimation(VS_SKINNED_INPUT input, uint nInstanceID : SV_InstanceID)
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

	output.position = mul(mul(float4(positionW, 1.0f), gmtxShadowView), gmtxShadowProjection);
	output.normal = positionW;
	output.TexC = input.TexC;
	return(output);

}

void PSShadowSkinnedAnimation(VS_SKINNED_OUTPUT input)
{
}

//----------------
VS_TEXTED_INSTANCING_OUTPUT VSShadowTextedInstancing(VS_TEXTED_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_TEXTED_INSTANCING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxShadowView), gmtxShadowProjection);
	output.normalW = mul(input.normal, (float3x3)gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.TexC = input.TexC;
	output.n = nInstanceID;
	return(output);
}

void PSShadowTextedInstancing(VS_TEXTED_INSTANCING_OUTPUT input)
{
	float4 color = gTexture.Sample(gsamAnisotropicWrap, input.TexC);
	clip(color.a - 0.1);
}
//---------------
[maxvertexcount(4)]
void ShadowGS(point VS_TEXTED_INSTANCING_OUTPUT input[1], inout TriangleStream<VS_TEXTED_INSTANCING_OUTPUT> outStream)
{
	float3 vUp = float3(0, 1, 0);
	float3 vLook = float3(gvShadowCameraPosition.xyz - input[0].positionW.xyz);
	vLook.y = 0;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fWidth = 150;
	float fHeight = 150;
	float4 pVertices[4] = {
	float4(input[0].positionW + fWidth * vRight - fHeight * vUp, 1.0f),
	float4(input[0].positionW + fWidth * vRight + fHeight * vUp, 1.0f),
	float4(input[0].positionW - fWidth * vRight - fHeight * vUp, 1.0f),
	float4(input[0].positionW - fWidth * vRight + fHeight * vUp, 1.0f)
	};
	float2 pUVs[4] = { float2(1.0f,1.0f),float2(1.0f,0.0f) ,float2(0.0f,1.0f) ,float2(0.0f,0.0f) };

	VS_TEXTED_INSTANCING_OUTPUT output;

	for (int i = 0; i < 4; ++i)
	{
		output.positionW = pVertices[i].xyz;
		output.position = mul(mul(float4(output.positionW, 1.0f), gmtxShadowView), gmtxShadowProjection);
		output.normalW = vLook;
		output.TexC = pUVs[i];
		output.n = input[0].n;
		outStream.Append(output);
	}
}
void PSShadowBillBoardInstancing(VS_TEXTED_INSTANCING_OUTPUT input)
{
	float3 uvw = float3(input.TexC, input.n);
	float4 color = gTexarray.Sample(gsamAnisotropicWrap, uvw);
	clip(color.a - 0.1);
}

//----------------
VS_TEXT_OUTPUT VSShadowTer(VS_TEXT_INPUT input)
{
	VS_TEXT_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxWorld);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxShadowView), gmtxShadowProjection);
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.TexC = input.TexC;
	output.TexC1 = input.TexC1*50.f;
	return(output);
}

[domain("tri")]
VS_TEXT_OUTPUT DSShadow(HS_CONSTANT_OUTPUT input, float3 uv: SV_DomainLocation, OutputPatch<HS_OUTPUT, 3> tri)
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
	output.position = mul(mul(float4(Position, 1.0f), gmtxShadowView), gmtxShadowProjection);

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

void PSShadowTer(VS_TEXT_OUTPUT input)
{
}