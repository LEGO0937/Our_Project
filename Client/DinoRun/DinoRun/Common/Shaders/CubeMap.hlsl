#include "Shaders.hlsl"

VS_SKYBOX_OUTPUT VSSkyBox(VS_SKYBOX_INPUT input)
{
	VS_SKYBOX_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return(output);
}

float4 PSSkyBox(VS_SKYBOX_OUTPUT input) : SV_TARGET
{
	float4 cColor = gCubeTexture.Sample(gsamLinearWrap, input.positionL);

	return(cColor);
}
