#pragma  once
#include <SDL_mutex.h>

#include <Utility/Typedefs.h>

namespace kaleidoscope
{
	// This class is a shell for the SDL Semaphore.
	class Semaphore
	{
	public:
		Semaphore(){ mSem = NULL; };
		~Semaphore(){ mSem = NULL; };


		/*
		* I32 kaleidoscope::Semaphore_init(kaleidoscope::Semaphore* m, U32 initialValue)
		*
		* In: Semaphore* m : A pointer to the mutex to initialize.
		* In: U32 initialValue : The value to start the semaphore with.
		* Out: I32 : 0 on success
		*			 -1 on failure
		*/
		static I32 Semaphore_init(Semaphore* s, U32 initialValue)
		{
			s->mSem = SDL_CreateSemaphore(initialValue);
			if (s->mSem != NULL)
			{
				return 0;
			} 
			else
			{
				return -1;
			}
		}

		/*
		 * void kaleidoscope::Semaphore_destroy(kaleidoscope::Semaphore* s)
		 *
		 * In: Semaphore* s : A pointer to the semaphore to destroy.
		 * Out: void :
		 */
		static void Semaphore_destroy(Semaphore* s)
		{
			if (s->mSem != NULL)
			{
				SDL_DestroySemaphore(s->mSem);
			}
		}


		/*
		* U32 Semaphore_value(Semaphore* s)
		*
		* In: Semaphore* s: A pointer to the semaphore to query
		* Out: U32 : The current value of the semaphore
		*/
		static U32 Semaphore_value(Semaphore* s)
		{
			return SDL_SemValue(s->mSem);
		}


		/*
		* I32 Semaphore_post(Semaphore* s)
		*
		* In: Semaphore* s: A pointer to the semaphore to increment.
		* Out: I32 : 0 on success.
		*			-1 on error.
		*/
		static I32 Semaphore_post(Semaphore* s)
		{
			return SDL_SemPost(s->mSem);
		}


		/*
		* I32 Semaphore_wait(Semaphore* s)
		*
		* In: Semaphore* s: A pointer to the semaphore to decrement.
		* Out: I32 : 0 on success.
		*			-1 on error.
		*
		* This is a blocking wait.
		*/
		static I32 Semaphore_wait(Semaphore* s)
		{
			return SDL_SemWait(s->mSem);
		}


		/*
		* I32 Semaphore_tryWait(Semaphore* s)
		*
		* In: Semaphore* s: A pointer to the semaphore to decrement.
		* Out: I32 : 0 on success.
		*			 1 if the semaphore would have to wait.
		*			-1 on error.
		*
		* This is a non blocking wait.
		*/
		static I32 Semaphore_tryWait(Semaphore* s)
		{
			return SDL_SemTryWait(s->mSem);
		}


		/*
		* I32 Semaphore_tryWait(Semaphore* s, U32 timeMS)
		*
		* In: Semaphore* s: A pointer to the semaphore to decrement.
		* In: U32 timeMS : The number of milliseconds the semaphore should block for.
		* Out: I32 : 0 on success.
		*			 1 if the semaphore could not acquire in the desired amount of time.
		*			-1 on error.
		*
		* This wait will attempt to acquire the semaphore for timeMS milliseconds and will either,
		*	return successfully when it acquires the semaphore or will reach its time limit and signal timeout.
		*/
		static I32 Semaphore_timeoutWait(Semaphore* s, U32 timeMS)
		{
			return SDL_SemWaitTimeout(s->mSem, timeMS);
		}

	private:
		SDL_sem* mSem;
	};
}