#include "Time.h"

#ifdef WINDOWS
#include <Windows.h>
#endif

#include "Utils/Reflection/Reflection.h"

namespace o2
{
	DECLARE_SINGLETON(Time);

	Time::Time():
		mApplicationTime(0), mLocalTime(0), mCurrentFrame(0), mDeltaTime(0), mFPS(0), mFPSSum(0),
		mFramesSum(0), mLastFPSCheckingTime(0)
	{}

	Time::~Time()
	{}

	void Time::Update(float dt)
	{
		mCurrentFrame++;
		mFramesSum += 1.0f;

		if (dt < FLT_EPSILON)
			return;

		mDeltaTime = dt;
		mApplicationTime += dt;
		mLocalTime += dt;

		mFPSSum += 1.0f / dt;
		if (mApplicationTime - mLastFPSCheckingTime > 0.3f)
		{
			mFPS = mFramesSum / (mApplicationTime - mLastFPSCheckingTime);
			mLastFPSCheckingTime = mApplicationTime;
			mFPSSum = 0;
			mFramesSum = 0;
		}
	}

	float Time::GetApplicationTime() const
	{
		return mInstance->mApplicationTime;
	}

	float Time::GetLocalTime() const
	{
		return mInstance->mLocalTime;
	}

	void Time::ResetLocalTime() const
	{
		mInstance->mLocalTime = 0;
	}

	void Time::SetLocalTime(float time) const
	{
		mInstance->mLocalTime = time;
	}

	int Time::GetCurrentFrame() const
	{
		return (int)mInstance->mCurrentFrame;
	}

	float Time::GetDeltaTime() const
	{
		return mInstance->mDeltaTime;
	}

	float Time::GetFPS() const
	{
		return mInstance->mFPS;
    }
    
#ifdef WINDOWS
    TimeStamp Time::CurrentTime() const
    {
        SYSTEMTIME tm;
        GetSystemTime(&tm);
        
        return TimeStamp(tm.wSecond, tm.wMinute, tm.wHour, tm.wDay, tm.wMonth, tm.wYear);
    }
#endif
    
#ifdef OSX
    TimeStamp Time::CurrentTime() const
    {
        return TimeStamp();
    }
#endif
}
