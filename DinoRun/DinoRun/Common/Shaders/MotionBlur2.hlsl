#include "Shaders.hlsl"


//����
cbuffer cbSettings : register(b0)
{
	int gBlurRadius;

	float w0;
	float w1;
	float w2;
	float w3;
	float w4;
	float w5;
	float w6;
	float w7;
	float w8;
	float w9;
	float w10;
};

static const int gMaxBlurRadius = 1;


Texture2D renderImage            : register(t2);
Texture2D velocityMap            : register(t3);

RWTexture2D<float4> gOutput : register(u0);

#define N 256
#define CacheSize (N + 2*gMaxBlurRadius)
groupshared float4 gCache[CacheSize];

[numthreads(N, 1, 1)]
void MotionBlurCS(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{

	int Blur = 10;

	// �ӵ� �ʿ��� �ӵ� ���� �� Z ���� ����
	float4 Velocity = velocityMap[dispatchThreadID.xy];

	Velocity.xy /= (float)Blur;

	int cnt = 1;
	float4 BColor;

	// ����� ������ �̹����� ����. a ���п� Z ���� ����ȴ�.
	float4 Out = renderImage[dispatchThreadID.xy];

	for (int i = cnt; i < Blur; i++)
	{
		// �ӵ� ������ ���� �ؼ� ��ġ�� ���� ����� ������ �̹����� ���� ������ ���� �� �ֽ��ϴ�.
		BColor = renderImage[(dispatchThreadID.xy + (Velocity.xy * (float)i))];

		// �ӵ� ���� Z ���� �ӵ� ���� ���⿡�ִ� �ؼ� ��ġ�� ���ø� �� ����� ������ �̹����� Z ���� ���Ѵ�. (���� 1)
		if (Velocity.a < BColor.a + 0.04f)
		{
			cnt++;
			Out += BColor;
		}
	}

	Out /= (float)cnt;

	gOutput[dispatchThreadID.xy] = Out;
}