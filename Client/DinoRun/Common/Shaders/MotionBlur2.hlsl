#include "Shaders.hlsl"


//블러링
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

	// 속도 맵에서 속도 벡터 및 Z 값을 얻을
	float4 Velocity = velocityMap[dispatchThreadID.xy];

	Velocity.xy /= (float)Blur;

	int cnt = 1;
	float4 BColor;

	// 장면의 렌더링 이미지를 얻을. a 성분에 Z 값이 저장된다.
	float4 Out = renderImage[dispatchThreadID.xy];

	for (int i = cnt; i < Blur; i++)
	{
		// 속도 벡터의 방향 텍셀 위치를 참조 장면의 렌더링 이미지의 색상 정보를 얻을 수 있습니다.
		BColor = renderImage[(dispatchThreadID.xy + (Velocity.xy * (float)i))];

		// 속도 맵의 Z 값과 속도 벡터 방향에있는 텍셀 위치를 샘플링 한 장면의 렌더링 이미지의 Z 값을 비교한다. (주의 1)
		if (Velocity.a < BColor.a + 0.04f)
		{
			cnt++;
			Out += BColor;
		}
	}

	Out /= (float)cnt;

	gOutput[dispatchThreadID.xy] = Out;
}