#include <SDL/SDL.h>

// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cTimer.h>

namespace BREATHE
{
	namespace UTIL
	{
		cTimer::cTimer()
		{
			iCount=0;

			fFPS=fLastTime=fUpdateInterval=fUpdateIntervalDivFPS=0.0f;
		}

		void cTimer::Init(unsigned int uiHz)
		{
			iCount=0;

			fFPS=(float)uiHz;
			fLastTime=GetTime();
			fUpdateInterval=2000.0f;
			fUpdateIntervalDivFPS=2000.0f/uiHz;
		}
		
		void cTimer::Update(float fCurrentTime)
		{
			iCount++;
			if(fCurrentTime - fLastTime > fUpdateInterval)
			{
				fFPS = iCount / (fCurrentTime - fLastTime) * 1000.0f;
				fLastTime=fCurrentTime;
				iCount=0;
			}
		}
	}
}