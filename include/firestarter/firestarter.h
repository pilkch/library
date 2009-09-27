#ifndef FIRESTARTER_H
#define FIRESTARTER_H

namespace firestarter
{
	// http://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard

	// Returns the directory that Firestarter has been installed into
	// "c:/program files/firestarter/"
	// "/shared/firestarter/"
	std::string GetFirestarterDirectory();

	// Returns a string to the shared profiles.xml file
	// GetFirestarterDirectory() + "profiles.xml"
	std::string GetProfilesPath();

	// Returns a string to the shared settings.xml file
	// GetFirestarterDirectory() + "settings.xml"
	std::string GetSettingsPath();

	// Returns a string to the packages directory
	// GetFirestarterDirectory() + "packages/"
	std::string GetPackagesDirectory();
}

#endif //FIRESTARTER_H
