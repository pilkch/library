#ifndef COPERATINGSYSTEM_H
#define COPERATINGSYSTEM_H

namespace BREATHE
{
	namespace OPERATINGSYSTEM
	{
		std::string GetOperatingSystemNameString();
		std::string GetOperatingSystemVersionString();
		std::string GetOperatingSystemFullString();

		int GetOperatingSystemVersionMajor();
		int GetOperatingSystemVersionMinor();
	}
}

#endif //COPERATINGSYSTEM_H
