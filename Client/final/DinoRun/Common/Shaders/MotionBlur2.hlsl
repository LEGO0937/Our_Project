#include "Shaders.hlsl"


//블러링
Texture2D renderImage            : register(t2);
Texture2D velocityMap            : register(t3);

RWTexture2D<float4> gOutput : register(u0);

#define N 256
[numthreads(N, 1, 1)]
void MotionBlurCS(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{

	int Blur = 15;

	// 속도 맵에서 속도 벡터 및 Z 값을 얻을
	float4 velocity = velocityMap[dispatchThreadID.xy];

	int cnt = 1;
	float4 color;

	// 장면의 렌더링 이미지를 얻음. a 성분에 Z 값이 저장된다.
	float4 Out = renderImage[dispatchThreadID.xy];

	for (int i = cnt; i < Blur; i++)
	{
		// 속도 벡터의 방향을 참조하여 렌더링된 이미지의 색상 정보를 받는다.
		color = renderImage[(dispatchThreadID.xy + (velocity.xy * (float)i))];

		{
			cnt++;
			Out += color;
		}
	}

	Out /= (float)cnt;

	gOutput[dispatchThreadID.xy] = Out;
}