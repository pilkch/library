# Set the minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Set the project name
PROJECT(library_unittest)

SET(CMAKE_SOURCE_DIR ./../../library/)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

SET(PROJECT_DIRECTORY "./")

SET(LIBRARY_INCLUDE "${CMAKE_SOURCE_DIR}/include/")
SET(LIBRARY_SRC "${CMAKE_SOURCE_DIR}/src/")

# Add definitions, compiler switches, etc.
ADD_DEFINITIONS("-Wall -W -Wextra -Werror -Wformat -Wformat-y2k -Winit-self -Wstack-protector -Wunknown-pragmas -Wundef -Wwrite-strings -Wno-unused-parameter -Wpointer-arith -Wno-switch -Woverloaded-virtual -Wno-stack-protector -Wmissing-include-dirs -Wuninitialized -std=c++11 -ffor-scope -O2")


IF("${CMAKE_SYSTEM}" MATCHES "Linux")
  ADD_DEFINITIONS("-D__LINUX__")
ENDIF()

IF(WIN32)
  ADD_DEFINITIONS("-DUNICODE -D_UNICODE")
  ADD_DEFINITIONS("-DNOMINMAX")
ENDIF()

ADD_DEFINITIONS("-DSPITFIRE_APPLICATION_NAME_LWR=\"library_unittest\"")
ADD_DEFINITIONS("-DBUILD_SPITFIRE_UNITTEST")

INCLUDE_DIRECTORIES(${LIBRARY_INCLUDE} "./include/")


# Files from library directory

MACRO(PREFIX_PATHS SOURCE_DIRECTORY SOURCE_FILES)
MESSAGE(STATUS "PREFIX_PATHS dir=${SOURCE_DIRECTORY}, files=${SOURCE_FILES}, ${ARGN}")
MESSAGE(a=${SOURCE_FILES})
  SET(OUTPUT_FILES "")
  SET(a ${OUTPUT_FILES})
  SET(OUTPUT_FILES ${a} "${SOURCE_DIRECTORY}${SOURCE_FILES}")
  FOREACH(SOURCE_FILE ${ARGN})
    MESSAGE(b=${SOURCE_FILE})
    SET(a ${OUTPUT_FILES})
    SET(OUTPUT_FILES ${a} "${SOURCE_DIRECTORY}${SOURCE_FILE}")
  ENDFOREACH()
MESSAGE(d=${OUTPUT_FILES})
ENDMACRO(PREFIX_PATHS)


SET(LIBRARY_BREATHE_SOURCE_DIRECTORY breathe/)
SET(LIBRARY_BREATHE_SOURCE_FILES
game/cAIPathFinder.cpp
)

PREFIX_PATHS(${LIBRARY_BREATHE_SOURCE_DIRECTORY} ${LIBRARY_BREATHE_SOURCE_FILES})
SET(OUTPUT_LIBRARY_BREATHE_SOURCE_FILES ${OUTPUT_FILES})



SET(LIBRARY_LIBTRASHMM_SOURCE_DIRECTORY libtrashmm/)
SET(LIBRARY_LIBTRASHMM_SOURCE_FILES
libtrashmm.cpp
)

PREFIX_PATHS(${LIBRARY_LIBTRASHMM_SOURCE_DIRECTORY} ${LIBRARY_LIBTRASHMM_SOURCE_FILES})
SET(OUTPUT_LIBRARY_LIBTRASHMM_SOURCE_FILES ${OUTPUT_FILES})



SET(LIBRARY_LIBXDGMM_SOURCE_DIRECTORY libxdgmm/)
SET(LIBRARY_LIBXDGMM_SOURCE_FILES
libxdgmm.cpp
)

PREFIX_PATHS(${LIBRARY_LIBXDGMM_SOURCE_DIRECTORY} ${LIBRARY_LIBXDGMM_SOURCE_FILES})
SET(OUTPUT_LIBRARY_LIBXDGMM_SOURCE_FILES ${OUTPUT_FILES})



SET(LIBRARY_SPITFIRE_SOURCE_DIRECTORY spitfire/)
SET(LIBRARY_SPITFIRE_SOURCE_FILES
spitfire.cpp
algorithm/algorithm.cpp algorithm/base64.cpp algorithm/crc.cpp algorithm/md5.cpp
math/cColour.cpp math/cVec2.cpp math/cVec3.cpp math/cVec4.cpp math/cMat3.cpp math/cMat4.cpp math/cPlane.cpp math/cQuaternion.cpp math/geometry.cpp math/math.cpp math/units.cpp
storage/document.cpp storage/file.cpp storage/filesystem.cpp storage/settings.cpp storage/xml.cpp
util/datetime.cpp util/log.cpp util/process.cpp util/string.cpp util/thread.cpp util/unittest.cpp)

IF(WIN32)
  SET(LIBRARY_SPITFIRE_SOURCE_FILES ${LIBRARY_SPITFIRE_SOURCE_FILES})
ENDIF()

PREFIX_PATHS(${LIBRARY_SPITFIRE_SOURCE_DIRECTORY} ${LIBRARY_SPITFIRE_SOURCE_FILES})
SET(OUTPUT_LIBRARY_SPITFIRE_SOURCE_FILES ${OUTPUT_FILES})



SET(LIBRARY_SOURCE_FILES ${OUTPUT_LIBRARY_SPITFIRE_SOURCE_FILES} ${OUTPUT_LIBRARY_BREATHE_SOURCE_FILES} ${OUTPUT_LIBRARY_LIBTRASHMM_SOURCE_FILES} ${OUTPUT_LIBRARY_LIBXDGMM_SOURCE_FILES}
)
PREFIX_PATHS(${LIBRARY_SRC} ${LIBRARY_SOURCE_FILES})
SET(OUTPUT_LIBRARY_SOURCE_FILES ${OUTPUT_FILES})
MESSAGE(final=${OUTPUT_LIBRARY_SOURCE_FILES})

#list all source files here
ADD_EXECUTABLE(${PROJECT_NAME} src/main.cpp ${OUTPUT_LIBRARY_SOURCE_FILES} ${PROJECT_SOURCE_FILES})


INCLUDE(FindPkgConfig)

SET(LIBRARIES
  Threads
)


# Some of the libraries have different names than their Find*.cmake name
SET(LIBRARIES_LINKED
  stdc++fs
  boost_iostreams
  boost_filesystem
  boost_system
  pthread
  xdg-basedir
)

FOREACH(LIBRARY_FILE ${LIBRARIES})
  Find_Package(${LIBRARY_FILE} REQUIRED)
ENDFOREACH(LIBRARY_FILE)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES_LINKED})
