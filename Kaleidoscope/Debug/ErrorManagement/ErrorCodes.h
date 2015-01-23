#pragma once

// Different errors that can occur when creating components.

namespace kaleidoscope
{
	enum ErrorCode
	{
		CODE_OK,
		CODE_POOL_FULL,
		CODE_NAME_TAKEN,
		CODE_IRRLICHT_ERROR,
		CODE_LUA_ERROR
	};
}