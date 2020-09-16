#include "Shaders.hlsl"

float wij(VS_TEXT_OUTPUT i, VS_TEXT_OUTPUT j)
{
	return dot((j.positionW - i.positionW), i.normalW);
}

VS_TEXT_OUTPUT VSTer(VS_TEXT_INPUT input)
{
	VS_TEXT_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxWorld);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.TexC = input.TexC;
	output.TexC1 = input.TexC1;
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
	float4 ShadowPosH = mul(float4(input.positionW, 1.0f), invViewProj);
	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	shadowFactor[0] = CalcShadowFactor(ShadowPosH);

	float4 baseAlbedo = gTexture.Sample(gsamLinearWrap,input.TexC);
	float4 detailedAlbedo = gDetailedTexture.Sample(gsamLinearWrap, input.TexC1);

	clip(baseAlbedo.a - 0.1f);

	float3 normalW = normalize(input.normalW);
	float4 color = saturate(0.4*(detailedAlbedo + baseAlbedo) + sLighting(input.positionW, normalW, shadowFactor)*0.6);

	float3 toEyeWorld = gvCameraPosition - input.positionW;
	float distToEye = length(toEyeWorld);


	if (fFogStart >= 5.0f)
	{
		float fogRate = saturate((distToEye - fFogStart) / fFogRange);
		color = lerp(color, f4FogColor, fogRate);
	}

	color.a = baseAlbedo.a;

	return color;
}