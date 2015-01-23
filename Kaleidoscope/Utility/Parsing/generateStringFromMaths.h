#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

#include <string>
#include <boost/format.hpp>


/*
 * std::string vec3ToString(kaleidoscope::math::vec3& v)
 *
 * Takes in a vec3 v and returns a string formated as (v.x, v.y, v.z)
 */
inline std::string vec3ToString(kaleidoscope::math::vec3& v)
{
	return (boost::format("(%1%, %2%, %3%)") % v.x % v.y % v.z).str();
}


/*
 * std::string vec4ToString(kaleidoscope::math::vec4& v)
 *
 * Takes in a vec4 v and returns a string formated as (v.x, v.y, v.z, v.w)
 */
inline std::string vec4ToString(kaleidoscope::math::vec4& v)
{
	return (boost::format("(%1%, %2%, %3%, %4%)") % v.x % v.y % v.z % v.w).str();
}


/*
 * std::string mat4ToString(kaleidoscope::math::mat4& m)
 *
 * Takes in a mat4 m and returns a column major formatted string
 *   (m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3])
 */
inline std::string mat4ToString(kaleidoscope::math::mat4& m)
{
	return (boost::format("(%1%, %2%, %3%, %4%, %5%, %6%, %7%, %8%, %9%, %10%, %11%, %12%, %13%, %14%, %15%, %16%)")
							% m[0][0] % m[0][1] % m[0][2] % m[0][3]
							% m[1][0] % m[1][1] % m[1][2] % m[1][3]
							% m[2][0] % m[2][1] % m[2][2] % m[2][3]
							% m[3][0] % m[3][1] % m[3][2] % m[3][3]).str();
}


/*
 * std::string quatToString(kaleidoscope::math::quat& q)
 *
 * Takes in a quat q and returns a string formated as (q.w, q.x, q.y, q.z)
 */
inline std::string quatToString(kaleidoscope::math::quat& q)
{
	return (boost::format("(%1%, %2%, %3%, %4%)") % q.w % q.x % q.y % q.z).str();
}
