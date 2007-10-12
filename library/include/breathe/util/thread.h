#ifndef CTHREAD_H
#define CTHREAD_H

namespace breathe
{
	namespace util
	{
		class cLockObject;

		inline void PauseThisThread(uint32_t milliseconds)
		{
			SDL_Delay(milliseconds);
		}

#ifdef PLATFORM_WINDOWS
#undef Yield
#endif

		inline void YieldThisThread()
		{
#ifdef PLATFORM_WINDOWS
			::Sleep(0);
#else
			sched_yield();
#endif
		}

		class cThread
		{
		public:
			cThread();
			~cThread();

			void Run();
			void Wait();
			void Kill();

			bool IsRunning() const;
			bool IsFinished() const;

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

		
		// *** cThread

		inline cThread::cThread() :
			thread(NULL)
		{
		}

		inline cThread::~cThread()
		{
			if (IsRunning()) Kill();
		}

		inline bool cThread::IsRunning() const
		{
			return thread != NULL;
		}

		inline bool cThread::IsFinished() const
		{
			return thread != NULL;
		}

		inline void cThread::Run()
		{
			thread = SDL_CreateThread(RunThreadFunction, this);
		}
		
		inline void cThread::Wait()
		{
			assert(thread);
			SDL_WaitThread(thread, NULL);
			thread = NULL;
		}

		inline void cThread::Kill()
		{
			assert(thread);
			SDL_KillThread(thread);
			thread = NULL;
		}

		// Not the most elegant method, but it works
		inline int cThread::RunThreadFunction(void* pData)
		{
			assert(pData);
			cThread* pThis = static_cast<cThread*>(pData);
			assert(pThis);
			return pThis->ThreadFunction();
		}


		// *** cMutex

		inline cMutex::cMutex()
		{
			mutex = SDL_CreateMutex();
			assert(mutex);
		}

		inline cMutex::~cMutex()
		{
			assert(mutex);
			SDL_DestroyMutex(mutex);
		}

		inline void cMutex::Lock()
		{
			assert(mutex);
			SDL_mutexP(mutex);
		}

		inline void cMutex::Unlock()
		{
			assert(mutex);
			SDL_mutexV(mutex);
		}


		// *** cLockObject

		inline cLockObject::cLockObject(cMutex* _mutex) :
			mutex(_mutex)
		{
			assert(mutex);
			mutex->Lock();
		}

		inline cLockObject::~cLockObject()
		{
			assert(mutex);
			mutex->Unlock();
		}
	}
}

#endif //CTHREAD_H
