#ifndef CTHREAD_H
#define CTHREAD_H

namespace breathe
{
	namespace util
	{
		class cLockObject;

		class cThread
		{
		public:
			cThread();
			~cThread();

			void Run();
			void Pause(uint32_t milliseconds);
			void Wait();
			void Kill();

		private:
			static int RunThreadFunction(void* pThis);
			virtual int ThreadFunction() = 0;

			SDL_Thread* thread;

			NO_COPY(cThread);
		};

		class cMutex
		{
		public:
			cMutex();
			~cMutex();

			friend cLockObject;

		private:
			void Lock();
			void Unlock();

			SDL_mutex* mutex;

			NO_COPY(cMutex);
		};

		class cLockObject
		{
		public:
			cLockObject(cMutex* mutex);
			~cLockObject();

		private:
			cMutex* mutex;

			NO_COPY(cLockObject);
		};
	}
}

#endif //CTHREAD_H
