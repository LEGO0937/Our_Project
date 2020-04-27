#include "Shaders.hlsl"

VS_FONT_OUTPUT VSFont(VS_FONT_INPUT input, uint vertexID : SV_VertexID)
{
	VS_FONT_OUTPUT output;

	// id 0 = 0000, uv = (0, 0)
	// id 1 = 0001, uv = (1, 0)
	// id 2 = 0010, uv = (0, 1)
	// id 3 = 0011, uv = (1, 1)
	float2 uv = float2(vertexID & 1, (vertexID >> 1) & 1);

	output.pos = float4(input.pos.x + (input.pos.z * uv.x), input.pos.y - (input.pos.w * uv.y), 0, 1);
	output.color = input.color;

	output.texCoord = float2(input.texCoord.x + (input.texCoord.z * uv.x), input.texCoord.y + (input.texCoord.w * uv.y));

	return output;
}

float4 PSFont(VS_FONT_OUTPUT input) : SV_TARGET
{
	
	float4 color = float4(input.color.rgb, gTexture.Sample(gsamLinearWrap, input.texCoord).a);
	clip(color.a - 0.03);
	return color;
}