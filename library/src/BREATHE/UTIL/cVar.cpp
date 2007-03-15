#if defined (_MSC_VER) && _MSC_VER <= 1200  // MSVC++ 6.0
#  pragma warning(disable: 4786)  
#endif

#include <sstream>
#include <string>
#include <list>
#include <map>
#include <iterator>
#include <vector>



#include <BREATHE/UTIL/cVar.h>
#include <BREATHE/UTIL/cConsole.h>
#include <BREATHE/UTIL/cLog.h>

namespace BREATHE
{
	cVar::cVar(const cVar& rhs)
	{
		//uiFlags=rhs.uiFlags;
		sValue=rhs.sValue;
	}

	cVar::cVar(std::string value, unsigned int flags)
	{
		//uiFlags=flags;
		sValue=value;
	}

	cVar::~cVar()
	{
		
	}


	cVar::operator bool () 
	{
		return "true"==sValue;
	}

	cVar::operator bool () const 
	{
		return "true"==sValue;
	}

	cVar::operator unsigned int () const 
	{
		std::stringstream t(sValue);
		unsigned int i;
		t>>i;
		return i;
	}
		
	cVar::operator int () const 
	{
		std::stringstream t(sValue);
		int i;
		t>>i;
		return i;
	}

	cVar::operator float () const 
	{
		std::stringstream t(sValue);
		float i;
		t>>i;
		return i;
	}
	
	cVar::operator std::string () const 
	{
		return sValue;
	}

	void cVar::operator=(const std::string &rhs)
	{
		sValue=rhs;
	}
}
