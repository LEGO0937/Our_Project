#include "Shaders.hlsl"

VS_WIREFRAME_OUTPUT VSWireFrame(VS_WIREFRAME_INPUT input)
{
	VS_WIREFRAME_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);

	return(output);
}

VS_WIREFRAME_OUTPUT VSWireFrameInstancing(VS_WIREFRAME_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_WIREFRAME_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].gmtxGameObject), gmtxView), gmtxProjection);
	return(output);
}

float4 PSWireFrame(VS_WIREFRAME_OUTPUT input) : SV_TARGET
{
	return(float4(1.0f, 0.0f, 0.0f, 1.0f));
}
