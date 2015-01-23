#pragma once

#include <SDL_mutex.h>

#include <Utility/Typedefs.h>

namespace kaleidoscope
{
	// This class is a shell for the SDL Mutex.
	class Mutex
	{
	public:
		Mutex(){ mMutex = NULL; };
		~Mutex(){ mMutex = NULL; };


		/*
		 * I32 kaleidoscope::Mutex_init(kaleidoscope::Mutex* m)
		 *
		 * In: Mutex* m : A pointer to the mutex to initialize.
		 * Out: I32 : 0 on success
		 *			 -1 on failure
		 */
		static I32 Mutex_init(Mutex* m)
		{
			m->mMutex = SDL_CreateMutex();
			if (m->mMutex != NULL)
			{
				return 0;
			}
			else
			{
				return -1;
			}
		};


		/*
		 * void kaleidoscope::Mutex_destroy(kaleidoscope::Mutex* m)
		 * 
		 * In: Mutex* m : A pointer to the mutex to destroy.
		 * Out: void :
		 */
		static void Mutex_destroy(Mutex* m)
		{
			if (m->mMutex != NULL)
			{
				SDL_DestroyMutex(m->mMutex);
			}
		};


		/*
		 * I32 kaleidoscope::Mutex_lock(kaleidoscope::Mutex* m)
		 *
		 * In: Mutex* m : A pointer to the mutex to aquire.
		 * Out: I32 : 0 on success.
		 *			 -1 on error.
		 *
		 * This is a blocking wait.
		 */
		static I32 Mutex_lock(Mutex* m)
		{
			return SDL_LockMutex(m->mMutex);
		};


		/*
		* I32 kaleidoscope::Mutex_trylock(kaleidoscope::Mutex* m)
		*
		* In: Mutex* m : A pointer to the mutex to aquire.
		* Out: I32 : 0 on success.
		*			-1 on error.
		*			 1 on busy lock.
		*
		* This is a non blocking wait.
		*/
		static I32 Mutex_trylock(Mutex* m)
		{
			return SDL_TryLockMutex(m->mMutex);
		};


		/*
		* I32 kaleidoscope::Mutex_unlock(kaleidoscope::Mutex* m)
		*
		* In: Mutex* m : A pointer to the mutex to release.
		* Out: I32 : 0 on success.
		*			-1 on error.
		*/
		static I32 Mutex_unlock(Mutex* m)
		{
			return SDL_UnlockMutex(m->mMutex);
		};


	private:
		SDL_mutex* mMutex;
	};
}

