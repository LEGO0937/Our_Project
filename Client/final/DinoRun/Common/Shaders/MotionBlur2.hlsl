#include "Shaders.hlsl"


//����
Texture2D renderImage            : register(t2);
Texture2D velocityMap            : register(t3);

RWTexture2D<float4> gOutput : register(u0);

#define N 256
[numthreads(N, 1, 1)]
void MotionBlurCS(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{

	int Blur = 15;

	// �ӵ� �ʿ��� �ӵ� ���� �� Z ���� ����
	float4 velocity = velocityMap[dispatchThreadID.xy];

	int cnt = 1;
	float4 color;

	// ����� ������ �̹����� ����. a ���п� Z ���� ����ȴ�.
	float4 Out = renderImage[dispatchThreadID.xy];

	for (int i = cnt; i < Blur; i++)
	{
		// �ӵ� ������ ������ �����Ͽ� �������� �̹����� ���� ������ �޴´�.
		color = renderImage[(dispatchThreadID.xy + (velocity.xy * (float)i))];

		{
			cnt++;
			Out += color;
		}
	}

	Out /= (float)cnt;

	gOutput[dispatchThreadID.xy] = Out;
}