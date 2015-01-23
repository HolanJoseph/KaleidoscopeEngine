#include <Utility/StringID/StringId.h>

namespace kaleidoscope
{

	static boost::unordered_map<StringID, const char *> gStringIDTable;
	static Semaphore internSem;
	static bool initialized = false;


	/*
	 * bool kaleidoscope::StringIDInit()
	 *
	 * Starts up the StringID system
	 * returns true if start up was successful or if startup was already completed.
	 * returns false if startup failed.
	 */
	bool StringIDInit()
	{
		if (!initialized)
		{
			initialized = true;

			I32 success = Semaphore::Semaphore_init(&internSem, 1);
			if (success == 0)
			{
				return true;
			}
			else
			{
				initialized = false;
				return false;
			}
		}

		return true;
	}


	/*
	 * bool kaleidoscope::StringIDShutdown()
	 *
	 * Shuts down the StringID system and clears the record of stored strings.
	 * returns true if shutdown was successful or if shutdown was already completed.
	 */
	bool StringIDShutdown()
	{
		if (initialized)
		{
			initialized = false;
			Semaphore::Semaphore_destroy(&internSem);
			gStringIDTable.clear();
			return true;
		}

		return true;
	}


	/*
	 * kaleidoscope::StringID kaleidoscope::hashCRC32(const char *str)
	 *
	 * Takes in a c str, computes its StringID sid, and returns sid
	 * THIS FUNCTION DOES NOT KEEP A RECORD OF THE STRING IN THE STRINGID SYSTEM.
	 */
	StringID hashCRC32(const char *str)
	{
		boost::crc_32_type crc32Computer;

		U32 numBytes = strlen(str) * sizeof(char);

		crc32Computer.process_bytes(str, numBytes);
		StringID retID = crc32Computer.checksum();

		return retID;
	}


	/*
	* kaleidoscope::StringID kaleidoscope::internString(const char *str)
	*
	* Takes in a c str, computes its StringID sid, stores a record of the string and returns sid
	*/
	StringID internString(const char *str)
	{
		StringID sid = hashCRC32(str);

		Semaphore::Semaphore_wait(&internSem);
		boost::unordered_map<StringID, const char *>::iterator it = gStringIDTable.find(sid);

		if (it == gStringIDTable.end())
		{
			// This string has not yet been added to the table.
			gStringIDTable[sid] = _strdup(str);
		}
		Semaphore::Semaphore_post(&internSem);

		return sid;
	}


	/*
	* const char * kaleidoscope::getString(kaleidoscope::StringID sid)
	*
	* Takes a StringID sid and returns the c string it represents if its value is stored.
	*/
	const char * getString(StringID sid)
	{
		return gStringIDTable[sid];
	}

}