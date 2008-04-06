#include <SDL/SDL.h>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cTimer.h>

namespace breathe
{
	namespace util
	{
		void cTimer::Init(unsigned int uiHz)
		{
			iCount=0;

			fFPS = (float)uiHz;
			fUpdateInterval = 2000.0f;
			fUpdateIntervalDivFPS = 2000.0f/uiHz;

			lastTime = GetTime();
		}
		
		void cTimer::Update(sampletime_t currentTime)
		{
			iCount++;
			if ((currentTime - lastTime) > fUpdateInterval)
			{
				fFPS = (iCount * ((currentTime - lastTime) / 1000.0f));
				lastTime = currentTime;
				iCount=0;
			}
		}
	}
}
