#pragma once

#include <cstdarg>
#include <SDL_log.h>

#include <Synchronization/Locks/Semaphore/Semaphore.h>

namespace kaleidoscope
{
	class SDLLogManager
	{
	public:

		// All of the categories that can be logged to.
		enum
		{
			LOG_CATEGORY_APPLICATION = SDL_LOG_CATEGORY_APPLICATION,
			LOG_CATEGORY_ERROR = SDL_LOG_CATEGORY_ERROR,
			LOG_CATEGORY_ASSERT = SDL_LOG_CATEGORY_ASSERT,
			LOG_CATEGORY_SYSTEM = SDL_LOG_CATEGORY_SYSTEM,
			LOG_CATEGORY_AUDIO = SDL_LOG_CATEGORY_AUDIO,
			LOG_CATEGORY_VIDEO = SDL_LOG_CATEGORY_VIDEO,
			LOG_CATEGORY_RENDER = SDL_LOG_CATEGORY_RENDER,
			LOG_CATEGORY_INPUT = SDL_LOG_CATEGORY_INPUT,
			LOG_CATEGORY_TEST = SDL_LOG_CATEGORY_TEST
		};

		// All of the priorities that we can set the logger to display.
		// Verbose is the lowest priority. 
		// Critical is the highest priority.
		//
		// By setting the priority to verbose a log message with any priority level will be displayed.
		// By setting the priority to warn only messages with level warn, error, and critical will be displayed.
		enum
		{
			LOG_PRIORITY_VERBOSE = SDL_LOG_PRIORITY_VERBOSE,
			LOG_PRIORITY_DEBUG = SDL_LOG_PRIORITY_DEBUG,
			LOG_PRIORITY_INFO = SDL_LOG_PRIORITY_INFO,
			LOG_PRIORITY_WARN = SDL_LOG_PRIORITY_WARN,
			LOG_PRIORITY_ERROR = SDL_LOG_PRIORITY_ERROR,
			LOG_PRIORITY_CRITICAL = SDL_LOG_PRIORITY_CRITICAL,
			NUM_LOG_PRIORITIES = SDL_NUM_LOG_PRIORITIES
		};

		// The SDLLogManager is a pseudo singleton class with an empty constructor and destructor.
		//  To initialize an instance of SDLLogManager you must call one of the init functions. Before 
		//	the destructor is called the shutdown() function should be called to release all of the classes
		//	data. As this class is controlling the SDL logging state manager only 1 instance of this class
		//  should exist at a time.
		SDLLogManager(){};
		~SDLLogManager(){};
		bool init(int allPriority);
		bool init(int appPriority, int errPriority, int assertPriority,
			int sysPriority, int audioPriority, int vidPriority,
			int renderPriority, int inputPriority, int testPriority);
		bool shutdown();

		// These functions display the provided string as with printf(). The priority level of the log is
		//	specified in the function name, and the category to be displayed is specified by the user in the
		//	category parameter. 
		void log(const char* fmt, ...);  // Log to the application category with a priority level of info.
		void logVerbose(int category, const char* fmt, ...);
		void logDebug(int category, const char* fmt, ...);
		void logInfo(int category, const char* fmt, ...);
		void logWarn(int category, const char* fmt, ...);
		void logError(int category, const char* fmt, ...);
		void logCritical(int category, const char* fmt, ...);


	protected:
	private:
	};
}