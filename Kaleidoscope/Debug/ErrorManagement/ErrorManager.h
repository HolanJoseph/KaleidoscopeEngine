#pragma once

#include <string>

#include <Debug/ErrorManagement/ErrorCodes.h>

namespace kaleidoscope
{
	class ErrorManager
	{
	private:
		ErrorCode mEC;
		std::string mES;

	public:
		ErrorManager() : mEC(CODE_OK), mES(std::string()) {};
		~ErrorManager() {};

		bool hasPendingError() const { return (mEC == CODE_OK ? false : true); };
		void clearError() { setError(CODE_OK, std::string()); };

		void setError(ErrorCode ec, const std::string& es)
		{
			mEC = ec;
			mES = es;
		};

		ErrorCode getErrorCode() const { return mEC; };
		std::string getErrorString() const { return mES; };
	};
}

