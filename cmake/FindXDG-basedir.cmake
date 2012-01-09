# Locate libxdg-basedir
# This module defines
# LIBXDGBASEDIR_LIBRARY
# LIBXDGBASEDIR_FOUND, if false, do not try to link to libxdg-basedir
# LIBXDGBASEDIR_INCLUDE_DIR, where to find the headers
#
# $LIBXDGBASEDIR_DIR is an environment variable that would
# correspond to the ./configure --prefix=$LIBXDGBASEDIR_DIR
#
# Created by Chris Pilkington


FIND_PATH(LIBXDGBASEDIR_INCLUDE_DIR basedir.h
    ${LIBXDGBASEDIR_DIR}/include
    $ENV{LIBXDGBASEDIR_DIR}/include
    $ENV{LIBXDGBASEDIR_DIR}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    /usr/freeware/include
)


MACRO(FIND_LIBXDGBASEDIR_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_LIBRARY(${MYLIBRARY}
    NAMES ${MYLIBRARYNAME}
    PATHS
    ${LIBXDGBASEDIR_DIR}/lib
    $ENV{LIBXDGBASEDIR_DIR}/lib
    $ENV{LIBXDGBASEDIR_DIR}
    $ENV{DELTA_ROOT}/ext/lib
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/lib
    /usr/lib
    /usr/lib64
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    /usr/freeware/lib64
)

ENDMACRO(FIND_LIBXDGBASEDIR_LIBRARY MYLIBRARY MYLIBRARYNAME)

SET(LIBRARYFIND_LIST libxdg-basedir)
FIND_LIBXDGBASEDIR_LIBRARY(LIBXDGBASEDIR_LIBRARY "${LIBRARYFIND_LIST}")

SET(LIBXDGBASEDIR_FOUND "NO")
IF(LIBXDGBASEDIR_LIBRARY AND LIBXDGBASEDIR_INCLUDE_DIR)
    SET(LIBXDGBASEDIR_FOUND "YES")
ENDIF(LIBXDGBASEDIR_LIBRARY AND LIBXDGBASEDIR_INCLUDE_DIR)

IF(LIBXDGBASEDIR_LIBRARY)
  MESSAGE(STATUS "libxdg-basedir was found")
ENDIF(LIBXDGBASEDIR_LIBRARY)
IF(!LIBXDGBASEDIR_LIBRARY)
  MESSAGE(STATUS "libxdg-basedir was not found")
ENDIF(!LIBXDGBASEDIR_LIBRARY)

