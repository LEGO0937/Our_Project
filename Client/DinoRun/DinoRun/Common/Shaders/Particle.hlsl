#include "Shaders.hlsl"

struct Particle
{
	float3 position;
	float3 velocity;
	float life;
};


cbuffer cbParticleCS : register(b1)
{
	float3 gPosition;
	float gGravity;
	float gElapsedTime;
	float gSize;
};
cbuffer cbParticleVS : register(b6)
{
	float3 f3Position;
	float fGravity;
	float fElapsedTime;
	float fSize;
};

RWStructuredBuffer<Particle> consumeBuf : register(u2);
StructuredBuffer<Particle> appendBuf : register(t1);

StructuredBuffer<Particle> gParticleInfos : register(t0);

float rand(float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}

[numthreads(1, 1, 1)]
void ParticleCS( uint3 id : SV_GroupID)
{
	Particle prevParticle = appendBuf[id.x];
	Particle curParticle;

	curParticle.position = prevParticle.position + (prevParticle.velocity * gElapsedTime);

	float3 flat = curParticle.position.xyz;
	flat.y = 0;
	if (length(prevParticle.velocity) != 0)
	{
		float3 force = normalize(flat) / length(curParticle.position);
		curParticle.velocity = prevParticle.velocity - ((force * gElapsedTime) * 0.5);
		curParticle.velocity.y -= gGravity * gElapsedTime;
	}
	else
	{
		curParticle.velocity = prevParticle.velocity;
	}
	curParticle.life = prevParticle.life - gElapsedTime;
	
	consumeBuf[id.x] = curParticle;
}




struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 TexC : TEXCOORD;
};

struct VS_PARTICLE_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 TexC : TEXCOORD;
	float size : SIZE;
	int n : NUM;
};

VS_PARTICLE_OUTPUT VSParticle(VS_PARTICLE_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_PARTICLE_OUTPUT output;
	output.positionW = gParticleInfos[nInstanceID].position + f3Position;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = input.normal;
	output.TexC = input.TexC;
	output.n = nInstanceID;
	output.size = fSize;
	return(output);
}
[maxvertexcount(4)]
void GSParticle(point VS_PARTICLE_OUTPUT input[1], inout TriangleStream<VS_PARTICLE_OUTPUT> outStream)
{
	float3 vUp = float3(0, 1, 0);
	float3 vLook = float3(gvCameraPosition.xyz - input[0].positionW.xyz);
	vLook.y = 0;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fWidth = input[0].size;
	float fHeight = input[0].size;
	float4 pVertices[4] = {
		float4(input[0].positionW + fWidth * vRight - fHeight * vUp, 1.0f),
		float4(input[0].positionW + fWidth * vRight + fHeight * vUp, 1.0f),
		float4(input[0].positionW - fWidth * vRight - fHeight * vUp, 1.0f),
		float4(input[0].positionW - fWidth * vRight + fHeight * vUp, 1.0f)
	};
	float2 pUVs[4] = { float2(1.0f,1.0f),float2(1.0f,0.0f) ,float2(0.0f,1.0f) ,float2(0.0f,0.0f) };

	VS_PARTICLE_OUTPUT output;

	for (int i = 0; i < 4; ++i)
	{
		output.positionW = pVertices[i].xyz;
		output.position = mul(mul(float4(pVertices[i].xyz, 1.0f), gmtxView), gmtxProjection);
		output.normalW = vLook;
		output.TexC = pUVs[i];
		output.n = input[0].n;
		output.size = input[0].size;
		outStream.Append(output);
	}
}


float4 PSParticle(VS_PARTICLE_OUTPUT input) : SV_TARGET
{
	float3 normalW = normalize(input.normalW);
	float4 baseAlbedo = gTexture.Sample(gsamAnisotropicWrap, input.TexC);
	float4 color = baseAlbedo;
	clip(baseAlbedo.a - 0.1);

	float3 toEyeWorld = gvCameraPosition - input.positionW;
	float distToEye = length(toEyeWorld);


	float fogRate = saturate((distToEye - fFogStart) / fFogRange);
	color = lerp(color, f4FogColor, fogRate);

	color.a = baseAlbedo.a;

	return(color);
}
