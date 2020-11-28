#pragma once
#include "../Shader.h"

//ui들을 인스턴싱하는 경우 이 구조체를 사용.
struct UI_INFO
{
	vector<XMFLOAT3> positions;
	vector<float> f_uvY;
	XMFLOAT2 meshSize;
	string textureName;
	XMFLOAT2 minUv = XMFLOAT2(0, 0);
	XMFLOAT2 maxUv = XMFLOAT2(0, 0);
};

struct CountDown_Info
{
	float time;
};
class TimeCountShader : public CUiShader
{
private:
	float								m_fSecond = 0.0f;
	int									m_iMinute = 0;
public:
	TimeCountShader();
	virtual ~TimeCountShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);

	float GetSecond() { return m_fSecond; }
	int GetMinute() { return m_iMinute; }
};

class GaugeShader : public CUiShader
{
public:
	GaugeShader();
	virtual ~GaugeShader();

	virtual void BuildObjects(void* pInformation);
	virtual void Update(float fTimeElapsed){}
	virtual void Update(float fTimeElapsed, void* pInformation);
};


class TrackCountShader : public CUiShader
{
private:

public:
	TrackCountShader();
	virtual ~TrackCountShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);
};

class RankCountShader : public CUiShader
{
private:

public:
	RankCountShader();
	virtual ~RankCountShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);
};

class VelocityCountShader : public CUiShader
{
private:

public:
	VelocityCountShader();
	virtual ~VelocityCountShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);
};

class CountDownShader : public CUiShader
{
private:
	float m_fScaleValue = 2.0f;
	float m_fFinishTime = 0.0f;
public:
	CountDownShader();
	virtual ~CountDownShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);
};


class DashBoardShader : public CUiShader
{
private:

public:
	DashBoardShader();
	virtual ~DashBoardShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);
};


class BackGroundShader : public CUiShader
{
private:

public:
	BackGroundShader();
	virtual ~BackGroundShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);
};

class ImageShader : public CUiShader
{
private:

public:
	ImageShader();
	virtual ~ImageShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);
};

class SelectImageShader : public CUiShader
{
private:

public:
	SelectImageShader();
	virtual ~SelectImageShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);
};

class IconShader : public CUiShader
{
private:

public:
	IconShader();
	virtual ~IconShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(void* pInformation);
};