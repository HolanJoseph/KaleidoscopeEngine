#pragma once

#include <Utility/Typedefs.h>
#include <SDL_thread.h>

// Threads share their parents global memory.

namespace kaleidoscope
{
	class Thread
	{
	public:
		// int function(void* data)
		// int (*ThreadFunction) (void* data)
		typedef SDL_ThreadFunction ThreadFunction;

		enum ThreadPriority
		{
			THREAD_PRIORITY_LOW,
			THREAD_PRIORITY_MEDIUM,
			THREAD_PRIORITY_HIGH
		};

		Thread(){ mThread = NULL; };
		~Thread(){ mThread = NULL; };

		// Creates a thread that runs the function specified by fn and passes it the argument data.
		//
		// returns 0 on success, -1 otherwise.
		static I32 Thread_create(Thread* t, const char* tName, ThreadFunction fn, void* data)
		{
			t->mThread = SDL_CreateThread(fn, tName, data);
			if (t->mThread != NULL)
			{
				return 0;
			} 
			else
			{
				return -1;
			}
		};

		// invalidates thread
		static void Thread_waitFor(Thread* t, I32* status)
		{
			if (t->mThread != NULL)
			{
				SDL_WaitThread(t->mThread, status);
			}
		};

		// Use this function to let a thread clean up on exit without intervention.
		// Signifies that the thread should not remain until Thread_waitFor() is called on it.
		// Useful for long running threads that need not be synchronized with.
		// 
		// No way to to recover thread status code.
		// Thread* t is not safe to access after call to detach.
		// Don't call both detach and waitFor on the same thread.
		//
		// t is a pointer sent to Thread_create()
		static void Thread_detach(Thread* t)
		{
			if (t->mThread != NULL)
			{
				SDL_DetachThread(t->mThread);
			}
		};

		// returns name or NULL if no name.
		static const char* Thread_getName(Thread* t)
		{
			return SDL_GetThreadName(t->mThread);
		};

		// returns 0 on success.
		static I32 Thread_setPriority(ThreadPriority priority)
		{
			SDL_ThreadPriority p;

			if (priority == THREAD_PRIORITY_LOW)
			{
				p = SDL_THREAD_PRIORITY_LOW;
			} 
			else if (priority == THREAD_PRIORITY_MEDIUM)
			{
				p = SDL_THREAD_PRIORITY_NORMAL;
			}
			else if (priority == THREAD_PRIORITY_HIGH)
			{
				p = SDL_THREAD_PRIORITY_HIGH;
			}

			return SDL_SetThreadPriority(p);
		};

		// return the ID of the current thread.
		static U32 Thread_getID()
		{
			return SDL_ThreadID();
		};


	private:
		SDL_Thread* mThread;
	};
}