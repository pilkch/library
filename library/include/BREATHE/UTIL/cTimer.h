#ifndef CTIMER_H
#define CTIMER_H

namespace BREATHE
{
	namespace UTIL
	{
		class cTimer
		{
		public:
			cTimer();
			
			void Init(unsigned int uiHz);
			void Update(float fCurrentTime);

			// Milliseconds per frame
			inline float GetMPF() { return fUpdateIntervalDivFPS; }

			// Frames per second
			inline float GetFPS() { return fFPS; }

		private:
			int iCount;

			float fFPS;
			float fLastTime;
			float fUpdateInterval;
			float fUpdateIntervalDivFPS;
		};

		/*class cProfiler
		{

			
			inline float GetMPF() { return fMPF; }


		private:
			float fMPF;

			float fTotalTime;

		};*/

		
		inline float GetTime()
		{
			//(float)timeGetTime;

			//return the milliseconds since we started
			return (float)SDL_GetTicks();
		}
	}
}

#endif //CTIMER_H
