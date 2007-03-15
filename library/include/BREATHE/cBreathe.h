/*************************************************************************
 *                                                                       *
 * Breathe Library, Copyright (C) 2006-2007 Chris Pilkington             *
 * All rights reserved.  Web: http://chris.iluo.net                      *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2.1 of the  *
 * License, or (at your option) any later version. The text of the GNU   *
 * General Public License is included with this library in the           *
 * file license.txt.                                                     *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 * See the file GPL.txt for more details.                                *
 *                                                                       *
 *************************************************************************/


#ifndef BREATHE_H
#define BREATHE_H

//Defines

//Platform
#if defined(WIN32) || defined(_MSC_VER) || defined(_BORLANDC_)
#define PLATFORM_WINDOWS
#elif defined(__LINUX__)
#define PLATFORM_LINUX
#elif defined(__MACOSX__)
#define PLATFORM_MAC
#else
!!! //We don't support this platform yet
#endif

//What type of build is this?
#ifdef NDEBUG
#define BUILD_RELEASE
#else
#define BUILD_DEBUG
#endif

//Utility types, objects etc.

namespace BREATHE
{
	//Constants
	const bool BAD=true;
	const bool GOOD=false;
	
	#define SIZE_TYPE unsigned long int

	const SIZE_TYPE STR_SMALL=256;
	const SIZE_TYPE STR_MEDIUM=2048;
	const SIZE_TYPE STR_LARGE=320000;

	#define STR_SIZE STR_LARGE

	#define BUFFER_SIZE_IN STR_LARGE
	#define BUFFER_SIZE_OUT STR_MEDIUM


	#define STR_END "\r\n"


	//Safe deleting functions
	template <class T> inline void SAFE_DELETE(T x)
	{ 
		delete x;
		x=NULL;
	}

	template <class T> inline void SAFE_DELETE_ARRAY(T x)
	{ 
		delete [] x;
		x=NULL;
	}
}

#endif //BREATHE_H