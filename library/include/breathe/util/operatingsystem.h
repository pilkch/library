#ifndef COPERATINGSYSTEM_H
#define COPERATINGSYSTEM_H

namespace breathe
{
	namespace operatingsystem
	{
		std::string GetOperatingSystemNameString();
		std::string GetOperatingSystemVersionString();
		std::string GetOperatingSystemFullString();

		int GetOperatingSystemVersionMajor();
		int GetOperatingSystemVersionMinor();

		std::string GetUserName();
	}
}

#endif //COPERATINGSYSTEM_H
