#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

// Other libraries
#include <sdl/sdl.h>

// Breathe
#include <breathe/breathe.h>
#include <breathe/util/thread.h>

namespace breathe
{
	namespace util
	{
		// *** cThread

		cThread::cThread() :
			thread(NULL)
		{
		}

		cThread::~cThread()
		{
			if (thread != NULL) Kill();
		}

		void cThread::Run()
		{
			thread = SDL_CreateThread(RunThreadFunction, this);
		}
		
		void cThread::Pause(uint32_t milliseconds)
		{
			SDL_Delay(milliseconds);
		}

		void cThread::Wait()
		{
			assert(thread);
			SDL_WaitThread(thread, NULL);
			thread = NULL;
		}

		void cThread::Kill()
		{
			assert(thread);
			SDL_KillThread(thread);
			thread = NULL;
		}

		// Not the most elegant method, but it works
		int cThread::RunThreadFunction(void* pData)
		{
			assert(pData);
			cThread* pThis = static_cast<cThread*>(pData);
			assert(pThis);
			return pThis->ThreadFunction();
		}

		// *** cMutex

		cMutex::cMutex()
		{
			mutex = SDL_CreateMutex();
			assert(mutex);
		}

		cMutex::~cMutex()
		{
			assert(mutex);
			SDL_DestroyMutex(mutex);
		}

		void cMutex::Lock()
		{
			assert(mutex);
			SDL_mutexP(mutex);
		}

		void cMutex::Unlock()
		{
			assert(mutex);
			SDL_mutexV(mutex);
		}

		// *** cLockObject

		cLockObject::cLockObject(cMutex* _mutex) :
			mutex(_mutex)
		{
			assert(mutex);
			mutex->Lock();
		}

		cLockObject::~cLockObject()
		{
			assert(mutex);
			mutex->Unlock();
		}
	}
}
