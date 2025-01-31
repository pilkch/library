/*************************************************************************
 *                                                                       *
 * Spitfire Library, Copyright (C) 2006 Onwards Chris Pilkington         *
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

// Defines
// BUILD_HTML_LOG - Allow logging to HTML
// BUILD_SDL - Build with SDL support
// NDEBUG - Release build (Defaults to debug build otherwise)

#ifndef SPITFIRE_H
#define SPITFIRE_H

// Standard headers
#include <cassert>
#include <string>

//#ifndef SPITFIRE_APPLICATION_NAME
//#error "SPITFIRE_APPLICATION_NAME must be defined"
//#endif

//#ifndef SPITFIRE_APPLICATION_NAME_LWR
//#error "SPITFIRE_APPLICATION_NAME_LWR must be defined"
//#endif

#ifdef CMEM_H
#error "Don't include mem.h directly, include spitfire.h instead"
#endif // CMEM_H

#ifndef BUILD_SDL
#define NO_SDL
#endif // !BUILD_SDL

// Architecture
#if defined(_M_IA64) || defined(__ia64__) || defined(_M_AMD64) || defined(__x86_64__) || defined(__LP64__) || defined(__amd64)
#define BUILD_PLATFORM_64
#endif

#if defined(__hppa__) || \
  defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
  (defined(__MIPS__) && defined(__MISPEB__)) || \
  (defined(__APPLE__) && defined(__MACH__)) || \
  defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
  defined(__sparc__)
#warning "Building big endian, this will probably be broken in many, many places"
#define BUILD_ENDIAN_BIG
#else
#define BUILD_ENDIAN_LITTLE
#endif

// Operating System
// Use the standard defines were possible:
// __WIN__, __LINUX__, __APPLE__
// as well as combination defines:
// PLATFORM_LINUX_OR_UNIX

#if defined(WIN32) || defined(__WIN__) || \
  defined(_MSC_VER) || defined(__CYGWIN32__) || defined(_BORLANDC_) || defined(__MINGW32__)
#ifndef __WIN__
#define __WIN__
#endif
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__LINUX__)
#ifndef __LINUX__
#define __LINUX__
#endif
#elif defined(__APPLE__) || defined(__apple__) || defined(MACOSX) || defined(__MACOSX__)
#ifndef __APPLE__
#define __APPLE__
#endif
#else
#error "Need some help identifying the platform!"
#endif

#if defined(__LINUX__) || defined(__APPLE__)
#define PLATFORM_LINUX_OR_UNIX
#endif

// What type of build is this?
#ifdef NDEBUG
#define BUILD_RELEASE
#else
#define BUILD_DEBUG
#endif

#ifdef _MSC_VER
#define COMPILER_MSVC
#elif defined(__GNUC__)
#define COMPILER_GCC
#else
#error "Unknown compiler"
#endif

#ifdef COMPILER_MSVC
#define COMPILER_MSVC_2015 1900
#define COMPILER_MSVC_2013 1800
#define COMPILER_MSVC_2012 1700
#define COMPILER_MSVC_2010 1600
#define COMPILER_MSVC_2008 1500
#define COMPILER_MSVC_2005 1400
#define COMPILER_MSVC_2003 1310
#endif

#ifdef __WIN__
#include <windows.h>

#ifdef BUILD_DEBUG
// Use CRT memory leak detection
#include <crtdbg.h>
#endif

#ifndef NOMINMAX
#error "NOMINMAX must be defined on Windows"
#endif

#ifdef _MBCS
#error "_MBCS should not be defined by your IDE"
#endif
#ifndef UNICODE
#error "UNICODE must be defined on Windows"
#endif

// The Windows headers require exceptions so we cannot disable them
//#ifdef _CPPUNWIND
//#error "Exceptions must be disabled"
//#endif
// The boost headers require exceptions so we cannot disable them
//#ifdef _CPPRTTI
//#error "RTTI must be disabled"
//#endif

#else

#ifdef UNICODE
#error "UNICODE must not be defined on this platform"
#endif

#define TEXT(s) s
#endif

#ifndef STRINGIFY
#define _STRINGIFY(value) #value
#define STRINGIFY(value)  _STRINGIFY(value)
#endif


// Thread local storage
// NOTE: This is only for POD types
// http://en.wikipedia.org/wiki/Thread-local_storage

// Again Visual Studio does it's own thing
#ifdef COMPILER_MSVC
#define THREAD_LOCAL __declspec( thread )
#else
#define THREAD_LOCAL __thread
#endif

// This is basically for documenting that a static/global variable is shared between threads (It shouldn't be used much at all, THREAD_LOCAL should basically be the default)
#define THREAD_GLOBAL


#ifdef COMPILER_GCC
#define GCC_VERSION (__GNUC__ * 100) + (__GNUC_MINOR__)
#define GCC_VERSION_4_7 407
#endif

// Visual Studio 2012 still doesn't support C++11
#if !defined(COMPILER_MSVC) || (defined(COMPILER_MSVC) && (_MSC_VER >= COMPILER_MSVC_2013))
#define BUILD_SPITFIRE_CPP11_AND_LATER
#endif

// Override keyword to flag virtual functions which are overridden from the base class
#if defined(COMPILER_MSVC) && (_MSC_VER < COMPILER_MSVC_2010)
#define override
#endif

#ifdef COMPILER_GCC
// Final keyword to flag virtual functions which are not allowed to be overridden in derived classes
#define final
#endif

#ifdef __WIN__
#undef interface
// Avoid problems with using the interface keyword
#define interface Interface
#endif

#ifndef BUILD_SPITFIRE_CPP11_AND_LATER
#define constexpr
#define noexcept
#endif

// Deprecation flags
#if defined(COMPILER_GCC) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define ATTRIBUTE_DEPRECATED __attribute__((deprecated))
#else
#define ATTRIBUTE_DEPRECATED
#endif


// *** Types

#ifdef NO_SDL
#include <cstdint>
#include <cstddef>
#else
// For our types (uint8_t, uint32_t, etc.)
#include <SDL/SDL.h>
#ifdef BUILD_PLATFORM_64
#include <stdint.h>
#endif
#endif

typedef float float32_t; // For reading/writing to and from files/network
typedef double float64_t; // For reading/writing to and from files/network

// NOTE: math.h also defines this
typedef float float_t;

// Apparently Visual Studio doesn't have ssize_t
#ifdef COMPILER_MSVC
#ifdef BUILD_PLATFORM_64
typedef int64_t ssize_t;
#else
typedef int32_t ssize_t;
#endif
#endif

#ifdef BUILD_DEBUG
#ifdef COMPILER_MSVC
// *** FluidStudios' memory leak detection
#ifndef FIRESTARTER
//#include <spitfire/util/mem.h>
#endif
#endif
#endif // BUILD_DEBUG


// Prevent a class from being copied
#ifdef BUILD_SPITFIRE_CPP11_AND_LATER
// Just add these manually
//#define NO_COPY(TYPE)
//  private:
//  TYPE(const TYPE&) = delete;
//  TYPE& operator=(const TYPE&) = delete;
//  TYPE(TYPE&&) noexcept = delete;
//  TYPE& operator=(TYPE&&) noexcept = delete;
#else
#define NO_COPY(T) \
  private: \
  T(const T&); \
  T& operator=(const T&);
#endif


// Like sizeof for arrays
#if defined(BUILD_SPITFIRE_CPP11_AND_LATER) && !defined(COMPILER_MSVC) || (defined(COMPILER_MSVC) && (_MSC_VER >= COMPILER_MSVC_2015))
template <typename T, size_t N>
constexpr size_t countof(const T (&array)[N]) noexcept
{
  return N;
}
#else
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define countof(array) (sizeof(ArraySizeHelper(array)))
#endif


// Utility types, objects etc.
namespace spitfire
{
  // For representing a file size on both 32 bit and 64 bit architectures
  typedef uint64_t filesize_t;

  // For specifying a time duration
  //typedef uint64_t sampletime_t; // Legacy
  typedef uint64_t durationms_t;


  // Safe deleting functions
  // These will delete your object/array and set your pointer to NULL so that we avoid double deletes
  // TODO: Replace this with std::unique_ptr<MyObject> p = std::make_unique<MyObject>();
  template <class T>
  inline void SAFE_DELETE(T& x)
  {
    delete x;
    x = nullptr;
  }

  // TODO: Replace this with std::vector
  template <class T>
  inline void SAFE_DELETE_ARRAY(T& x)
  {
    delete [] x;
    x = nullptr;
  }
}


// Static Assert
// NOTE: These are compile time so we do them in both debug and release
#define STATIC_ASSERT(expression, szDescription) static_assert(expression, szDescription)

#ifdef ASSERT
#undef ASSERT
#endif

#ifdef BUILD_DEBUG

#ifdef BUILD_HTML_LOG

// Assert
namespace spitfire
{
  void InformativeAssert(bool b, const char* szAssert, const char* szFile, const char* szFunction, size_t line);
}

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define ASSERT(p) spitfire::InformativeAssert(p, #p, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#else
// Assert
namespace spitfire
{
  #ifdef COMPILER_MSVC
  inline void DoAssert(bool b, const char* szAssert, const char* szFile, const char* szFunction, size_t line)
  {
    (void)szAssert;
    (void)szFile;
    (void)szFunction;
    (void)line;

    // TODO: Log the file, function and line

    if (!b) __debugbreak();
  }
  #endif
}

#ifdef COMPILER_MSVC

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define ASSERT(p) spitfire::DoAssert(p, #p, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define ASSERT_MSG(p, message, ...) if (!(p)) { LOGERROR(message); ASSERT(p); }

#else
#define ASSERT(b) assert(b)
#define ASSERT_MSG(p, message, ...) if (!(p)) { std::cerr<<message<<std::endl; ASSERT(p); }
#endif // COMPILER_MSVC

#endif // BUILD_HTML_LOG

#else
#define ASSERT(...)
#endif // BUILD_DEBUG

#endif // SPITFIRE_H
