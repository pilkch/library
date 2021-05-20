# Set the minimum cmake version
cmake_minimum_required(VERSION 3.10)

SET(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

IF(UNIX)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
ELSE()
# Set the C++ standard
#SET(CMAKE_CXX_STANDARD 14)
#SET(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_STANDARD 20)
ENDIF()

# Set the project name
PROJECT(library_unittest)

SET(CMAKE_SOURCE_DIR ./../../library/)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

SET(PROJECT_DIRECTORY "./")

SET(LIBRARY_INCLUDE "${CMAKE_SOURCE_DIR}/include/")
SET(LIBRARY_SRC "${CMAKE_SOURCE_DIR}/src/")

IF(UNIX)
# Add definitions, compiler switches, etc.
ADD_DEFINITIONS("-std=c++17 -Wall -W -Wextra -Werror -Wformat -Wformat-y2k -Winit-self -Wstack-protector -Wunknown-pragmas -Wundef -Wwrite-strings -Wno-unused-parameter -Wpointer-arith -Wno-switch -Woverloaded-virtual -Wno-stack-protector -Wmissing-include-dirs -Wuninitialized -O2")
ENDIF()

IF(UNIX)
  ADD_DEFINITIONS("-D__LINUX__")
  ADD_DEFINITIONS("-DBUILD_NETWORK_TLS")
ENDIF()

IF(WIN32)
  ADD_DEFINITIONS("-DUNICODE -D_UNICODE")
  ADD_DEFINITIONS("-DNOMINMAX")

  ADD_DEFINITIONS("/std:c++latest")
ENDIF()

ADD_DEFINITIONS("-DSPITFIRE_APPLICATION_NAME_LWR=\"library_unittest\"")
ADD_DEFINITIONS("-DSPITFIRE_APPLICATION_COMPANY_NAME=\"The Company\"")

INCLUDE_DIRECTORIES(${LIBRARY_INCLUDE})


# Files from library directory

MACRO(PREFIX_PATHS SOURCE_DIRECTORY SOURCE_FILES)
MESSAGE(STATUS "PREFIX_PATHS dir=${SOURCE_DIRECTORY}, files=${SOURCE_FILES}, ${ARGN}")
  SET(OUTPUT_FILES "")
  SET(a ${OUTPUT_FILES})
  SET(OUTPUT_FILES ${a} "${SOURCE_DIRECTORY}${SOURCE_FILES}")
  FOREACH(SOURCE_FILE ${ARGN})
    SET(a ${OUTPUT_FILES})
    SET(OUTPUT_FILES ${a} "${SOURCE_DIRECTORY}${SOURCE_FILE}")
  ENDFOREACH()
ENDMACRO(PREFIX_PATHS)


SET(LIBRARY_BREATHE_SOURCE_DIRECTORY breathe/)
SET(LIBRARY_BREATHE_SOURCE_FILES
game/cAIPathFinder.cpp
)

PREFIX_PATHS(${LIBRARY_BREATHE_SOURCE_DIRECTORY} ${LIBRARY_BREATHE_SOURCE_FILES})
SET(OUTPUT_LIBRARY_BREATHE_SOURCE_FILES ${OUTPUT_FILES})



IF(UNIX)

SET(LIBRARY_LIBTRASHMM_SOURCE_DIRECTORY libtrashmm/)
SET(LIBRARY_LIBTRASHMM_SOURCE_FILES
libtrashmm.cpp
)

PREFIX_PATHS(${LIBRARY_LIBTRASHMM_SOURCE_DIRECTORY} ${LIBRARY_LIBTRASHMM_SOURCE_FILES})
SET(OUTPUT_LIBRARY_LIBTRASHMM_SOURCE_FILES ${OUTPUT_FILES})



SET(LIBRARY_LIBGNUTLSMM_SOURCE_DIRECTORY libgnutlsmm/)
SET(LIBRARY_LIBGNUTLSMM_SOURCE_FILES
gnutlsmm.cpp
)

PREFIX_PATHS(${LIBRARY_LIBGNUTLSMM_SOURCE_DIRECTORY} ${LIBRARY_LIBGNUTLSMM_SOURCE_FILES})
SET(OUTPUT_LIBRARY_LIBGNUTLSMM_SOURCE_FILES ${OUTPUT_FILES})



SET(LIBRARY_LIBXDGMM_SOURCE_DIRECTORY libxdgmm/)
SET(LIBRARY_LIBXDGMM_SOURCE_FILES
libxdgmm.cpp
)

PREFIX_PATHS(${LIBRARY_LIBXDGMM_SOURCE_DIRECTORY} ${LIBRARY_LIBXDGMM_SOURCE_FILES})
SET(OUTPUT_LIBRARY_LIBXDGMM_SOURCE_FILES ${OUTPUT_FILES})

ELSE()

SET(LIBRARY_LIBWIN32MM_SOURCE_DIRECTORY libwin32mm/)
SET(LIBRARY_LIBWIN32MM_SOURCE_FILES
#libwin32mm.cpp
#aboutdialog.cpp bitmap.cpp controls.cpp dialog.cpp filebrowse.cpp
filesystem2.cpp
#keys.cpp maindialog.cpp progressdialog.cpp taskbar.cpp taskdialog.cpp window.cpp
)

PREFIX_PATHS(${LIBRARY_LIBWIN32MM_SOURCE_DIRECTORY} ${LIBRARY_LIBWIN32MM_SOURCE_FILES})
SET(OUTPUT_LIBRARY_LIBWIN32MM_SOURCE_FILES ${OUTPUT_FILES})

ENDIF()


SET(LIBRARY_SPITFIRE_SOURCE_DIRECTORY spitfire/)
SET(LIBRARY_SPITFIRE_SOURCE_FILES
spitfire.cpp
algorithm/algorithm.cpp algorithm/base64.cpp algorithm/crc.cpp algorithm/md5.cpp
communication/http.cpp communication/network.cpp
math/cColour.cpp math/cCurve.cpp math/cVec2.cpp math/cVec3.cpp math/cVec4.cpp math/cMat3.cpp math/cMat4.cpp math/cPlane.cpp math/cQuaternion.cpp math/geometry.cpp math/math.cpp math/units.cpp
storage/file.cpp storage/filesystem.cpp storage/json.cpp storage/document.cpp storage/settings.cpp storage/xml.cpp
util/datetime.cpp util/lang.cpp util/log.cpp util/poll.cpp util/process.cpp util/string.cpp util/timer.cpp util/thread.cpp util/weather_bom.cpp
)

IF(WIN32)
  SET(LIBRARY_SPITFIRE_SOURCE_FILES ${LIBRARY_SPITFIRE_SOURCE_FILES})
ENDIF()

PREFIX_PATHS(${LIBRARY_SPITFIRE_SOURCE_DIRECTORY} ${LIBRARY_SPITFIRE_SOURCE_FILES})
SET(OUTPUT_LIBRARY_SPITFIRE_SOURCE_FILES ${OUTPUT_FILES})



SET(LIBRARY_SOURCE_FILES ${OUTPUT_LIBRARY_SPITFIRE_SOURCE_FILES} ${OUTPUT_LIBRARY_BREATHE_SOURCE_FILES} ${OUTPUT_LIBRARY_LIBTRASHMM_SOURCE_FILES} ${OUTPUT_LIBRARY_LIBGNUTLSMM_SOURCE_FILES} ${OUTPUT_LIBRARY_LIBXDGMM_SOURCE_FILES}  ${OUTPUT_LIBRARY_LIBWIN32MM_SOURCE_FILES}
)
PREFIX_PATHS(${LIBRARY_SRC} ${LIBRARY_SOURCE_FILES})
SET(OUTPUT_LIBRARY_SOURCE_FILES ${OUTPUT_FILES})
MESSAGE(final=${OUTPUT_LIBRARY_SOURCE_FILES})

# Test source files
SET(TEST_SOURCE_FILES
ai_path_finder_test.cpp
algorithm_test.cpp base64_test.cpp crc_test.cpp
#dispatcher_test.cpp notifications_test.cpp # Require gtkmm
html_test.cpp
geometry_test.cpp math_test.cpp units_test.cpp
lang_test.cpp process_test.cpp settings_test.cpp spitfire_test.cpp string_test.cpp thread_test.cpp
network_test.cpp
weather_bom_test.cpp
breathe_vehicle_test.cpp
main.cpp
)
PREFIX_PATHS(src/ ${TEST_SOURCE_FILES})
SET(OUTPUT_TEST_SOURCE_FILES ${OUTPUT_FILES})

ADD_EXECUTABLE(${PROJECT_NAME} ${OUTPUT_TEST_SOURCE_FILES} ${OUTPUT_LIBRARY_SOURCE_FILES})


IF(WIN32)
target_compile_features(library_unittest PUBLIC cxx_std_20)
ENDIF()

# HACK: Pass the C++ version to the compiler
if(MSVC)
    target_compile_options(library_unittest PUBLIC "/Zc:__cplusplus")
endif()


INCLUDE(FindPkgConfig)

SET(LIBRARIES
  Threads
)


IF(UNIX)
SET(STANDARD_LIBRARIES
  stdc++fs
  pthread
  gnutls
  gtest
)
SET(XDG_LIBRARIES
  xdg-basedir
)
ENDIF()

# Some of the libraries have different names than their Find*.cmake name
SET(LIBRARIES_LINKED
  ${STANDARD_LIBRARIES}
  ${XDG_LIBRARIES}
)

FOREACH(LIBRARY_FILE ${LIBRARIES})
  Find_Package(${LIBRARY_FILE} REQUIRED)
ENDFOREACH(LIBRARY_FILE)

#need to link to some other libraries ? just add them here
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES_LINKED})

