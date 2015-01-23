#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;


/*
* kaleidoscope::math::vec2 stringToVec2(const char * s, bool& success)
*
* Takes a c string formated as (float x, float y) and returns a vec2 representation as (x, y)
* Takes a reference to a boolean that will be true if the function was able to successfully parse 2 numbers otherwise false.
*/
inline kaleidoscope::math::vec2 stringToVec2(const char * s, bool& success)
{
	std::vector< std::string > tokens;
	boost::split(tokens, s, boost::is_any_of(","));

	for (std::vector< std::string >::iterator str = tokens.begin(); str != tokens.end(); ++str)
	{
		boost::trim_if((*str), boost::is_any_of("() "));
	}

	if (tokens.size() == 2)
	{
		success = true;
		return kaleidoscope::math::vec2(std::stof(tokens[0]), std::stof(tokens[1]));
	}
	else
	{
		success = false;
		return kaleidoscope::math::vec2();
	}
}


/*
 * kaleidoscope::math::vec3 stringToVec3(const char * s, bool& success)
 *
 * Takes a c string formated as (float x, float y, float z) and returns a vec3 representation as (x, y, z)
 * Takes a reference to a boolean that will be true if the function was able to successfully parse 3 numbers otherwise false.
 */
inline kaleidoscope::math::vec3 stringToVec3(const char * s, bool& success)
{
	std::vector< std::string > tokens;
	boost::split(tokens, s, boost::is_any_of(","));

	for (std::vector< std::string >::iterator str = tokens.begin(); str != tokens.end(); ++str)
	{
		boost::trim_if((*str), boost::is_any_of("() "));
	}
	
	if (tokens.size() == 3)
	{
		success = true;
		return kaleidoscope::math::vec3(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]));
	}
	else
	{
		success = false;
		return kaleidoscope::math::vec3();
	}
}


/*
 * kaleidoscope::math::vec4 stringToVec4(const char * s, bool& success)
 *
 * Takes a c string formated as (float x, float y, float z, float w) and returns a vec4 representation as (x, y, z, w)
 * Takes a reference to a boolean that will be true if the function was able to successfully parse 4 numbers otherwise false.
 */
inline kaleidoscope::math::vec4 stringToVec4(const char * s, bool& success)
{
	std::vector< std::string > tokens;
	boost::split(tokens, s, boost::is_any_of(","));

	for (std::vector< std::string >::iterator str = tokens.begin(); str != tokens.end(); ++str)
	{
		boost::trim_if((*str), boost::is_any_of("() "));
	}

	if (tokens.size() == 4)
	{
		success = true;
		return kaleidoscope::math::vec4(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
	}
	else
	{
		success = false;
		return kaleidoscope::math::vec4();
	}
}


/*
* kaleidoscope::math::quat stringToQuat(const char * s, bool& success)
*
* Takes a c string formated as (float w, float x, float y, float z) and returns a quat representation as (w, x, y, z)
* Takes a reference to a boolean that will be true if the function was able to successfully parse 4 numbers otherwise false.
*/
inline kaleidoscope::math::quat stringToQuat(const char * s, bool& success)
{
	std::vector< std::string > tokens;
	boost::split(tokens, s, boost::is_any_of(","));

	for (std::vector< std::string >::iterator str = tokens.begin(); str != tokens.end(); ++str)
	{
		boost::trim_if((*str), boost::is_any_of("() "));
	}

	if (tokens.size() == 4)
	{
		success = true;
		return kaleidoscope::math::quat(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
	}
	else
	{
		success = false;
		return kaleidoscope::math::quat();
	}
}


/*
* kaleidoscope::math::mat4 stringToMat4(const char * s, bool& success)
*
* Takes a c string formated as a column major representation 
*   (m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3])
*   and returns a mat4 (column major) representation as 
*   (m[0][0], m[1][0], m[2][0], m[3][0],
*	 m[0][1], m[1][1], m[2][1], m[3][1],
*	 m[0][2], m[1][2], m[2][2], m[3][2],
*	 m[0][3], m[1][3], m[2][3], m[3][3])
* Takes a reference to a boolean that will be true if the function was able to successfully parse 16 numbers otherwise false.
*/
inline kaleidoscope::math::mat4 stringToMat4(const char * s, bool& success)
{
	std::vector< std::string > tokens;
	boost::split(tokens, s, boost::is_any_of(","));

	for (std::vector< std::string >::iterator str = tokens.begin(); str != tokens.end(); ++str)
	{
		boost::trim_if((*str), boost::is_any_of("() "));
	}

	if (tokens.size() == 16)
	{
		success = true;
		return kaleidoscope::math::mat4(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]),
										std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6]), std::stof(tokens[7]),
										std::stof(tokens[8]), std::stof(tokens[9]), std::stof(tokens[10]), std::stof(tokens[11]),
										std::stof(tokens[12]), std::stof(tokens[13]), std::stof(tokens[14]), std::stof(tokens[15]));
	}
	else
	{
		success = false;
		return kaleidoscope::math::mat4();
	}
}