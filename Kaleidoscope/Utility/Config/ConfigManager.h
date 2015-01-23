#pragma once

#include <Utility/Typedefs.h>
#include <windows.h>

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/optional.hpp>

class ConfigManager
{
public:
	ConfigManager();
	~ConfigManager();

	bool startUp(const char* filename);
	bool shutDown();

	bool getBoolean(const char* section, const char* variable, bool defaultValue) const;
	I32 getInt(const char* section, const char* variable, I32 defaultValue) const;
	F32 getFloat(const char* section, const char* variable, F32 defaultValue) const;
	wchar_t* getWString(const char* section, const char* variable, const char* defaultValue) const;
	boost::optional<const boost::property_tree::ptree&> getPtree(const char* section) const;
	

private:
	boost::property_tree::ptree mConfigTree;
};

