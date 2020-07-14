#include "Shaders.hlsl"



CREATE_VEL_MAP_OUTPUT VSVelocitySkinnedAnimation(VS_SKINNED_INPUT input)
{
	CREATE_VEL_MAP_OUTPUT output;

	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	float3 prevPositionW = float3(0.0f, 0.0f, 0.0f);
	float3 normalW = float3(0.0f, 0.0f, 0.0f);

	matrix mtxVertexToBoneWorld;
	matrix mtxPrevVertexToBoneWorld;

	float4 curPos;
	float4 prevPos;

	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		mtxPrevVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxPrevBoneTransforms[input.indices[i]]);
		positionW += (input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld)).xyz;
		prevPositionW += (input.weights[i] * mul(float4(input.position, 1.0f), mtxPrevVertexToBoneWorld)).xyz;
		normalW += input.weights[i] * mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	}

	curPos = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	prevPos = mul(mul(float4(prevPositionW, 1.0f), gmtxPrevView), gmtxProjection);

	
	float3 dir = curPos.xyz - prevPos.xyz;

	float a = dot(normalize(dir), normalize(normalW));

	if (a < 0.0f)
		output.position = prevPos;
	else
		output.position = curPos;

	output.direction.xy = (curPos.xy / curPos.w - prevPos.xy / prevPos.w) * 0.5f;

	// 마지막으로 텍셀의 오프셋 값이되기 때문에 Y 방향을 반대 방향으로하는
	output.direction.y *= -1.0f;

	// 장면의 Z 값을 계산하기위한 매개 변수
	output.direction.z = output.position.z;
	output.direction.w = output.position.w;

	output.TexC = input.TexC;
	return output;
}

CREATE_VEL_MAP_OUTPUT VSVelocitySkinnedInstancingAnimation(VS_SKINNED_INPUT input, uint nInstanceID : SV_InstanceID)
{
	CREATE_VEL_MAP_OUTPUT output;

	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	float3 prevPositionW = float3(0.0f, 0.0f, 0.0f);
	float3 normalW = float3(0.0f, 0.0f, 0.0f);

	matrix mtxVertexToBoneWorld;
	matrix mtxPrevVertexToBoneWorld;

	float4 curPos;
	float4 prevPos;

	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gSkinedGameObjectInfos[nInstanceID].gpmtxInstancedBoneTransforms[input.indices[i]]);
		mtxPrevVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gSkinedGameObjectInfos[nInstanceID].gpmtxInstancedPrevBoneTransforms[input.indices[i]]);
		positionW += (input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld)).xyz;
		prevPositionW += (input.weights[i] * mul(float4(input.position, 1.0f), mtxPrevVertexToBoneWorld)).xyz;
		normalW += input.weights[i] * mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	}

	curPos = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	prevPos = mul(mul(float4(prevPositionW, 1.0f), gmtxPrevView), gmtxProjection);

	float3 dir = curPos.xyz - prevPos.xyz;

	float a = dot(normalize(dir), normalize(normalW));

	if (a < 0.0f)
		output.position = prevPos;
	else
		output.position = curPos;

	output.direction.xy = (curPos.xy / curPos.w - prevPos.xy / prevPos.w) * 0.5f;

	// 마지막으로 텍셀의 오프셋 값이되기 때문에 Y 방향을 반대 방향으로하는
	output.direction.y *= -1.0f;

	// 장면의 Z 값을 계산하기위한 매개 변수
	output.direction.z = output.position.z;
	output.direction.w = output.position.w;

	output.TexC = input.TexC;
	return output;
}

float4 PSVelocityMap(CREATE_VEL_MAP_OUTPUT input) : SV_TARGET
{
	float4 color = gTexture.Sample(gsamAnisotropicWrap, input.TexC);
	clip(color.a - 0.1);

	float4 output;

	output.xy = input.direction.xy;

	// 미사용
	output.z = 1.0f;

	// Z 값을 계산
	output.w = input.direction.z / input.direction.w;

	return output;
}

//----------------
CREATE_VEL_MAP_OUTPUT VSVelocityTextedInstancing(VS_TEXTED_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID)
{

	CREATE_VEL_MAP_OUTPUT output;

	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	float3 prevPositionW = float3(0.0f, 0.0f, 0.0f);
	float3 normalW = float3(0.0f, 0.0f, 0.0f);

	matrix mtxWorld;
	matrix mtxPrevWorld;

	float4 curPos;
	float4 prevPos;


	mtxWorld = gGameObjectInfos[nInstanceID].gmtxGameObject;
	mtxPrevWorld = gGameObjectInfos[nInstanceID].gmtxPrevGameObject;
	positionW = mul(float4(input.position, 1.0f), mtxWorld).xyz;
	prevPositionW = mul(float4(input.position, 1.0f), mtxPrevWorld).xyz;
	normalW = mul(input.normal, (float3x3)mtxWorld).xyz;


	curPos = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	prevPos = mul(mul(float4(prevPositionW, 1.0f), gmtxPrevView), gmtxProjection);

	float3 dir = curPos.xyz - prevPos.xyz;

	float a = dot(normalize(dir), normalize(normalW));

	if (a < 0.0f)
		output.position = prevPos;
	else
		output.position = curPos;

	output.direction.xy = (curPos.xy / curPos.w - prevPos.xy / prevPos.w) * 0.5f;

	// 마지막으로 텍셀의 오프셋 값이되기 때문에 Y 방향을 반대 방향으로하는
	output.direction.y *= -1.0f;

	// 장면의 Z 값을 계산하기위한 매개 변수
	output.direction.z = output.position.z;
	output.direction.w = output.position.w;

	output.TexC = input.TexC;
	return output;
}

//---------------

VS_TEXTED_INSTANCING_OUTPUT VSVelocityBillBoardInstancing(VS_TEXTED_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_TEXTED_INSTANCING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxShadowView), gmtxShadowProjection);
	output.normalW = mul(input.normal, (float3x3)gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.TexC = input.TexC;
	output.n = fBillBoardSize;
	return(output);
}

[maxvertexcount(4)]
void VelocityGS(point VS_TEXTED_INSTANCING_OUTPUT input[1], inout TriangleStream<CREATE_VEL_MAP_OUTPUT> outStream)
{
	float3 vUp = float3(0, 1, 0);
	float3 vLook = float3(gvShadowCameraPosition.xyz - input[0].positionW.xyz);
	vLook.y = 0;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fWidth = input[0].n;
	float fHeight = input[0].n * 2;
	float4 pVertices[4] = {
		float4(input[0].positionW + fWidth * vRight, 1.0f),
		float4(input[0].positionW + fWidth * vRight + fHeight * vUp, 1.0f),
		float4(input[0].positionW - fWidth * vRight, 1.0f),
		float4(input[0].positionW - fWidth * vRight + fHeight * vUp, 1.0f)
	};
	float2 pUVs[4] = { float2(1.0f,1.0f),float2(1.0f,0.0f) ,float2(0.0f,1.0f) ,float2(0.0f,0.0f) };

	CREATE_VEL_MAP_OUTPUT output;
	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	float3 prevPositionW = float3(0.0f, 0.0f, 0.0f);
	float3 normalW = float3(0.0f, 0.0f, 0.0f);

	float4 curPos;
	float4 prevPos;

	float3 dir;
	float a;
	for (int i = 0; i < 4; ++i)
	{
		curPos = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
		prevPos = mul(mul(float4(positionW, 1.0f), gmtxPrevView), gmtxProjection);

		normalW = vLook;
		dir = curPos.xyz - prevPos.xyz;

		a = dot(normalize(dir), normalize(normalW));

		if (a < 0.0f)
			output.position = prevPos;
		else
			output.position = curPos;

		output.direction.xy = (curPos.xy / curPos.w - prevPos.xy / prevPos.w) * 0.5f;

		output.direction.y *= -1.0f;

		output.direction.z = output.position.z;
		output.direction.w = output.position.w;

		output.TexC = pUVs[i];
		outStream.Append(output);
	}
}

//----------------
CREATE_VEL_MAP_OUTPUT VSVelocityTer(VS_TEXT_INPUT input)
{
	CREATE_VEL_MAP_OUTPUT output;

	float3 positionW = float3(0.0f, 0.0f, 0.0f);
	float3 prevPositionW = float3(0.0f, 0.0f, 0.0f);
	float3 normalW = float3(0.0f, 0.0f, 0.0f);

	matrix mtxWorld;
	matrix mtxPrevWorld;

	float4 curPos;
	float4 prevPos;


	mtxWorld = gmtxWorld;
	mtxPrevWorld = gmtxPrevWorld;
	positionW = mul(float4(input.position, 1.0f), mtxWorld).xyz;
	prevPositionW = mul(float4(input.position, 1.0f), mtxPrevWorld).xyz;
	normalW = mul(input.normal, (float3x3)mtxWorld).xyz;


	curPos = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	prevPos = mul(mul(float4(prevPositionW, 1.0f), gmtxPrevView), gmtxProjection);

	float3 dir = curPos.xyz - prevPos.xyz;

	float a = dot(normalize(dir), normalize(normalW));

	if (a < 0.0f)
		output.position = prevPos;
	else
		output.position = curPos;

	output.direction.xy = (curPos.xy / curPos.w - prevPos.xy / prevPos.w) * 0.5f;

	// 마지막으로 텍셀의 오프셋 값이되기 때문에 Y 방향을 반대 방향으로하는
	output.direction.y *= -1.0f;

	// 장면의 Z 값을 계산하기위한 매개 변수
	output.direction.z = output.position.z;
	output.direction.w = output.position.w;

	output.TexC = input.TexC;
	return output;
}

