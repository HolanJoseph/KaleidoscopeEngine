#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <cctype>
#include <cstring>
#include <algorithm>

namespace kaleidoscope
{
	/*
	 * kaleidoscope::StringID kaleidoscope::lowerize(const char* str)
	 *
	 * Takes in a c string str, converts all characters in str to lowercase, interns it, and returns the StringID
	 */
	inline StringID lowerize(const char* str)
	{
		const size_t n = strlen(str);
		char * strLowerArray = new char[n + 1];
		strLowerArray[n] = '\0';
		std::transform(&str[0], &str[n], &strLowerArray[0], ::tolower);
		return internString(strLowerArray);
	}
}