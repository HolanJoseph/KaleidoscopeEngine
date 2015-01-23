#include <Debug/Logging/SDLLogManager.h>

namespace kaleidoscope
{
	static Semaphore logSem;

	/*
	* bool kaleidoscope::SDLLogManager::init(int allPriority)
	*
	* In: int allPriority : The minimum report level for all categories.
	*
	* If allPriority is 4 a call to logVerbose(priority 1) will not print as its priority is to low.
	*
	* 1 - Verbose
	* 2 - Debug
	* 3 - Info
	* 4 - Warn
	* 5 - Error
	* 6 - Critical
	*/
	bool SDLLogManager::init(int allPriority)
	{
		SDL_LogSetAllPriority(static_cast<SDL_LogPriority>(allPriority));

		int success = Semaphore::Semaphore_init(&logSem, 1);
		if (success == 0)
		{
			return true;
		} 
		else
		{
			return false;
		}
	}

	/*
	* bool SDLLogManager::init(int appPriority, int errPriority, int assertPriority,
	*						   int sysPriority, int audioPriority, int vidPriority,
	*						   int renderPriority, int inputPriority, int testPriority)
	*
	* In: ALL : The priority level for each individual category. SEE SDLLogManager::init(int allPriority) for more info.
	*/
	bool SDLLogManager::init(int appPriority, int errPriority, int assertPriority,
		int sysPriority, int audioPriority, int vidPriority,
		int renderPriority, int inputPriority, int testPriority)
	{
		int success = Semaphore::Semaphore_init(&logSem, 1);
		if (success == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool SDLLogManager::shutdown()
	{
		Semaphore::Semaphore_destroy(&logSem);
		return true;
	}

	/*
	* void kaleidoscope::SDLLogManager::log(const char* fmt, ...)
	*
	* In: const char* fmt : A format string in the style of printf.
	* In: ... : the format values.
	* 
	* Priority 3
	* Category Application
	*/
	void SDLLogManager::log(const char* fmt, ...)
	{
		Semaphore::Semaphore_wait(&logSem);
		// Create the variable argument list to pass to sdl.
		va_list fmtArgList;
		va_start(fmtArgList, fmt);

		SDL_LogMessageV(LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, fmtArgList);

		va_end(fmtArgList);
		Semaphore::Semaphore_post(&logSem);
	}


	/*
	* void kaleidoscope::SDLLogManager::logVerbose(int category, const char* fmt, ...)
	*
	* In: int category : The category to print to.
	* In: const char* fmt : A format string in the style of printf.
	* In: ... : the format values.
	*
	* Priority 1
	*/
	void SDLLogManager::logVerbose(int category, const char* fmt, ...)
	{
		Semaphore::Semaphore_wait(&logSem);
		// Create the variable argument list to pass to sdl.
		va_list fmtArgList;
		va_start(fmtArgList, fmt);

		SDL_LogMessageV(category, SDL_LOG_PRIORITY_VERBOSE, fmt, fmtArgList);

		va_end(fmtArgList);
		Semaphore::Semaphore_post(&logSem);
	}


	/*
	* void kaleidoscope::SDLLogManager::logDebug(int category, const char* fmt, ...)
	*
	* In: int category : The category to print to.
	* In: const char* fmt : A format string in the style of printf.
	* In: ... : the format values.
	*
	* Priority 2
	*/
	void SDLLogManager::logDebug(int category, const char* fmt, ...)
	{
		Semaphore::Semaphore_wait(&logSem);
		// Create the variable argument list to pass to sdl.
		va_list fmtArgList;
		va_start(fmtArgList, fmt);

		SDL_LogMessageV(category, SDL_LOG_PRIORITY_DEBUG, fmt, fmtArgList);

		va_end(fmtArgList);
		Semaphore::Semaphore_post(&logSem);
	}


	/*
	* void kaleidoscope::SDLLogManager::logInfo(int category, const char* fmt, ...)
	*
	* In: int category : The category to print to.
	* In: const char* fmt : A format string in the style of printf.
	* In: ... : the format values.
	*
	* Priority 3
	*/
	void SDLLogManager::logInfo(int category, const char* fmt, ...)
	{
		Semaphore::Semaphore_wait(&logSem);
		// Create the variable argument list to pass to sdl.
		va_list fmtArgList;
		va_start(fmtArgList, fmt);

		SDL_LogMessageV(category, SDL_LOG_PRIORITY_INFO, fmt, fmtArgList);

		va_end(fmtArgList);
		Semaphore::Semaphore_post(&logSem);
	}


	/*
	* void kaleidoscope::SDLLogManager::logWarn(int category, const char* fmt, ...)
	*
	* In: int category : The category to print to.
	* In: const char* fmt : A format string in the style of printf.
	* In: ... : the format values.
	*
	* Priority 4
	*/
	void SDLLogManager::logWarn(int category, const char* fmt, ...)
	{
		Semaphore::Semaphore_wait(&logSem);
		// Create the variable argument list to pass to sdl.
		va_list fmtArgList;
		va_start(fmtArgList, fmt);

		SDL_LogMessageV(category, SDL_LOG_PRIORITY_WARN, fmt, fmtArgList);

		va_end(fmtArgList);
		Semaphore::Semaphore_post(&logSem);
	}


	/*
	* void kaleidoscope::SDLLogManager::logError(int category, const char* fmt, ...)
	*
	* In: int category : The category to print to.
	* In: const char* fmt : A format string in the style of printf.
	* In: ... : the format values.
	*
	* Priority 5
	*/
	void SDLLogManager::logError(int category, const char* fmt, ...)
	{
		Semaphore::Semaphore_wait(&logSem);
		// Create the variable argument list to pass to sdl.
		va_list fmtArgList;
		va_start(fmtArgList, fmt);

		SDL_LogMessageV(category, SDL_LOG_PRIORITY_ERROR, fmt, fmtArgList);

		va_end(fmtArgList);
		Semaphore::Semaphore_post(&logSem);
	}


	/*
	* void kaleidoscope::SDLLogManager::logCritical(int category, const char* fmt, ...)
	*
	* In: int category : The category to print to.
	* In: const char* fmt : A format string in the style of printf.
	* In: ... : the format values.
	*
	* Priority 6
	*/
	void SDLLogManager::logCritical(int category, const char* fmt, ...)
	{
		Semaphore::Semaphore_wait(&logSem);
		// Create the variable argument list to pass to sdl.
		va_list fmtArgList;
		va_start(fmtArgList, fmt);

		SDL_LogMessageV(category, SDL_LOG_PRIORITY_CRITICAL, fmt, fmtArgList);

		va_end(fmtArgList);
		Semaphore::Semaphore_post(&logSem);
	}
}