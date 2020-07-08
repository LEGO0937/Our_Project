#include "Shaders.hlsl"



VS_TEXTED_INSTANCING_OUTPUT VSTextedInstancing(VS_TEXTED_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_TEXTED_INSTANCING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gGameObjectInfos[nInstanceID].gmtxGameObject);
	output.TexC = input.TexC;
	output.n = fBillBoardSize;

	return(output);
}
[maxvertexcount(4)]
void GS(point VS_TEXTED_INSTANCING_OUTPUT input[1], inout TriangleStream<VS_TEXTED_INSTANCING_OUTPUT> outStream)
{
	float3 vUp = float3(0, 1, 0);
	float3 vLook = float3(gvCameraPosition.xyz - input[0].positionW.xyz);
	vLook.y = 0;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fWidth = input[0].n;
	float fHeight = input[0].n*2;
	float4 pVertices[4] = {
		float4(input[0].positionW + fWidth * vRight, 1.0f),
		float4(input[0].positionW + fWidth * vRight + fHeight * vUp, 1.0f),
		float4(input[0].positionW - fWidth * vRight, 1.0f),
		float4(input[0].positionW - fWidth * vRight + fHeight * vUp, 1.0f)
	};
	float2 pUVs[4] = { float2(1.0f,1.0f),float2(1.0f,0.0f) ,float2(0.0f,1.0f) ,float2(0.0f,0.0f) };

	VS_TEXTED_INSTANCING_OUTPUT output;

	for (int i = 0; i < 4; ++i)
	{
		output.positionW = pVertices[i].xyz;
		output.position = mul(mul(float4(pVertices[i].xyz, 1.0f), gmtxView), gmtxProjection);
		output.normalW = vLook;
		output.TexC = pUVs[i];
		output.n = input[0].n;
		outStream.Append(output);
	}
}


float4 PSTextedInstancing(VS_TEXTED_INSTANCING_OUTPUT input) : SV_TARGET
{
	float3 normalW = normalize(input.normalW);
	float3 uvw = float3(input.TexC, input.n);
	float4 color = gTexarray.Sample(gsamAnisotropicWrap,uvw);
	clip(color.a - 0.1);
	color = color * 0.5 + (Lighting(input.positionW, normalW)*0.5);
	return(color);
}