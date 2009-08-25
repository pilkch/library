#include <string>
#include <list>

#include <firestarter/firestarter.h>

namespace firestarter
{
	// Returns the directory that Firestarter has been installed into
	// "c:/program files/firestarter/"
	std::string GetFirestarterDirectory()
	{
		return "H:/dev/chris/firestarter/";
	}

	std::string GetPackagesDirectory()
	{
		return "H:/dev/chris/";
	}

	// Returns a string to the shared profiles.xml file
	std::string GetProfilesPath()
	{
		return GetFirestarterDirectory() + "profiles.xml";
	}

	// Returns a string to the shared settings.xml file
	std::string GetSettingsPath()
	{
		return GetFirestarterDirectory() + "settings.xml";
	}
}
