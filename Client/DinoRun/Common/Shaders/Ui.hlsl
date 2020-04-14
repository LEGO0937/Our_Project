#include "Shaders.hlsl"

//-------------------ÅØ½ºÃ³o UI---------------------------------
VS_UI_OUTPUT VSUi(VS_UI_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_UI_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), gGameUiInfos[nInstanceID].gmtxGameUi);
	output.TexC = input.TexC;
	output.num = gGameUiInfos[nInstanceID].numUi;

	return(output);
}

float4 PSUi(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 diffuseAlbedo = gTexture.Sample(gsamAnisotropicWrap,input.TexC);
	clip(diffuseAlbedo.a - 0.1f);

	float4 color = diffuseAlbedo;
	color.a = diffuseAlbedo.a;
	return color;
}

VS_UI_OUTPUT VSUiNumber(VS_UI_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_UI_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), gGameUiInfos[nInstanceID].gmtxGameUi);
	
	output.num = gGameUiInfos[nInstanceID].numUi;

	output.TexC.x = input.TexC.x + (output.num % 10) * 0.1;
	output.TexC.y = input.TexC.y + ((output.num / 10) * 0.5);
	return(output);
}

VS_UI_OUTPUT VSUiGuage(VS_UI_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_UI_OUTPUT output;
	output.position = float4(input.position, 1.0f);
	if (output.position.x > 0)
		output.position.x = -output.position.x + output.position.x * 2 * gGameUiInfos[nInstanceID].numUi;
	output.position = mul(output.position, gGameUiInfos[nInstanceID].gmtxGameUi);

	output.num = gGameUiInfos[nInstanceID].numUi;

	output.TexC.x = input.TexC.x;
	output.TexC.y = input.TexC.y;
	return(output);
}