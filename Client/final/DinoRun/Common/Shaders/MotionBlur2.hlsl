#include "Shaders.hlsl"


//����
Texture2D renderTarget            : register(t2);
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

	// ����� ������ �̹��� �ҷ�����. a ���п� Z ���� ����ȴ�.
	// �ӵ����ۿ� ������ �ȼ���ġ���� �ҷ�����
	float4 output = renderTarget[dispatchThreadID.xy];

	for (int i = cnt; i < Blur; i++)
	{
		// �ӵ� ������ ������ �����Ͽ� �������� �̹����� ���� ������ �޴´�.
		color = renderTarget[(dispatchThreadID.xy + (velocity.xy * (float)i))];

		cnt++;
		output += color;
		
	}

	output /= (float)cnt;

	gOutput[dispatchThreadID.xy] = output;
}