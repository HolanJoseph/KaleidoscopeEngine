#include <Utility/Config/ConfigManager.h>

using std::string;
using boost::property_tree::ptree;

ConfigManager::ConfigManager()
{
}


ConfigManager::~ConfigManager()
{
}

bool ConfigManager::startUp(const char* filename)
{
	read_ini(filename, mConfigTree);
	return true;
}

bool ConfigManager::shutDown()
{
	return true;
}

bool ConfigManager::getBoolean(const char* section, const char* variable, bool defaultValue) const
{
	string tmpPath = section;
	tmpPath = tmpPath + "." + variable;

	return mConfigTree.get(tmpPath, defaultValue);
}

I32 ConfigManager::getInt(const char* section, const char* variable, I32 defaultValue) const
{
	string tmpPath = section;
	tmpPath = tmpPath + "." + variable;

	return mConfigTree.get(tmpPath, defaultValue);
}

F32 ConfigManager::getFloat(const char* section, const char* variable, F32 defaultValue) const
{
	string tmpPath = section;
	tmpPath = tmpPath + "." + variable;

	return mConfigTree.get(tmpPath, defaultValue);
}

wchar_t* ConfigManager::getWString(const char* section, const char* variable, const char* defaultValue) const
{
	string tmpPath = section;
	tmpPath = tmpPath + "." + variable;

	std::string tmpString(defaultValue);
	std::string retString = mConfigTree.get(tmpPath, tmpString);
	U32 strSize = retString.size() + 1;

	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t* wcstring = new wchar_t[strSize];
	mbstowcs_s(&convertedChars, wcstring, strSize, retString.c_str(), _TRUNCATE);

	return wcstring;
}

boost::optional<const boost::property_tree::ptree&> ConfigManager::getPtree(const char* section) const
{
	return mConfigTree.get_child_optional(section);
}
