#pragma once

#include <Utility/Typedefs.h>

#include <Synchronization/Locks/Semaphore/Semaphore.h>


#include <boost/crc.hpp>
#include <boost/unordered_map.hpp>

namespace kaleidoscope
{

	typedef U32 StringID;

	static const StringID NULLNAME = 2226387543;

	extern bool StringIDInit();
	extern bool StringIDShutdown();
	extern StringID internString(const char *str);
	extern StringID hashCRC32(const char *str);
	extern const char * getString(StringID sid);

}