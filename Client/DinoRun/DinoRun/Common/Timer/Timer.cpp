#include "../stdafx.h"
#include "Timer.h"

CGameTimer::CGameTimer()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
	mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

float CGameTimer::TotalTime()const
{
	
	//stoptime:종료시점,  멈춰있었을 시 그동안 멈춰있던 시간을 빼고 시작시간을 빼면
	//플레이타임이 나옴
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}

	//현재시점에서도 위와 마찬가지
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}
}

float CGameTimer::DeltaTime()const
{
	return (float)mDeltaTime;
}

void CGameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void CGameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


	
	//재시작 지점에서 멈췄던 지점을 빼면 중지한 시간이 나옴
	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void CGameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

void CGameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

	
	mPrevTime = mCurrTime;

	
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

