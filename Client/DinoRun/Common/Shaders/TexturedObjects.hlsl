#include "Shaders.hlsl"

VS_UI_OUTPUT VSTexture(VS_UI_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_UI_OUTPUT output;
	float3 positionW = (float3)mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	output.TexC = input.TexC;
	output.num = nInstanceID;
	return(output);
}

float4 PSTexture(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 color = gTexture.Sample(gsamAnisotropicWrap, input.TexC);
	clip(color.a - 0.1);
	return(color);
}



VS_TEXTED_INSTANCING_OUTPUT VSTextedInstancing(VS_TEXTED_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_TEXTED_INSTANCING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.TexC = input.TexC;
	output.n = nInstanceID;
	return(output);
}

float4 PSTextedInstancing(VS_TEXTED_INSTANCING_OUTPUT input) : SV_TARGET
{
	float3 normalW = normalize(input.normalW);
	float4 color = gTexture.Sample(gsamAnisotropicWrap, input.TexC);
	clip(color.a - 0.1);
	color = color * 0.5 + (Lighting(input.positionW, normalW)*0.5);
	return(color);
}
