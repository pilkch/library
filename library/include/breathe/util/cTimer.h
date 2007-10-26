#ifndef CTIMER_H
#define CTIMER_H

namespace breathe
{
	namespace util
	{
		class cTimer
		{
		public:
			cTimer();
			
			void Init(unsigned int uiHz);
			void Update(sampletime_t currentTime);

			// Milliseconds per frame
			inline float GetMPF() const { return fUpdateIntervalDivFPS; }

			// Frames per second
			inline float GetFPS() const { return fFPS; }

		private:
			int iCount;

			uint32_t lastTime;

			float fFPS;
			float fUpdateInterval;
			float fUpdateIntervalDivFPS;
		};
		
		inline sampletime_t GetTime()
		{
			//return the milliseconds since we started
			return SDL_GetTicks();
		}
	}
}

#endif //CTIMER_H
