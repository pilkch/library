# Locate libnotifymm
# This module defines
# LIBNOTIFYMM_LIBRARY
# LIBNOTIFYMM_FOUND, if false, do not try to link to libnotifymm
# LIBNOTIFYMM_INCLUDE_DIR, where to find the headers
#
# $LIBNOTIFYMM_DIR is an environment variable that would
# correspond to the ./configure --prefix=$LIBNOTIFYMM_DIR
#
# Created by Chris Pilkington


# Sorry, but libnotifymm includes are broken
# libnotifymm-1.0/libnotifymm.h includes libnotifymm/init.h, libnotifymm/notify.h and libnotifymm/notification.h which exist in a sub directory
# I don't get why they don't just call libnotifymm-1.0 libnotifymm and get rid of the sub directory
# Anyway, my solution for the moment is:
# Move the contents of libnotifymm-1.0/libnotifymm/ to libnotifymm-1.0/
# Rename libnotifymm-1.0/ to libnotifymm/
FIND_PATH(LIBNOTIFYMM_INCLUDE_DIR libnotifymm/libnotifymm.h
    ${LIBNOTIFYMM_DIR}/include
    $ENV{LIBNOTIFYMM_DIR}/include
    $ENV{LIBNOTIFYMM_DIR}
    ${DELTA3D_EXT_DIR}/inc
    $ENV{DELTA_ROOT}/ext/inc
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /usr/include/cal3d
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/include
    /usr/freeware/include
)


MACRO(FIND_LIBNOTIFYMM_LIBRARY MYLIBRARY MYLIBRARYNAME)

FIND_LIBRARY(${MYLIBRARY}
    NAMES ${MYLIBRARYNAME}
    PATHS
    ${LIBNOTIFYMM_DIR}/lib
    $ENV{LIBNOTIFYMM_DIR}/lib
    $ENV{LIBNOTIFYMM_DIR}
    ${DELTA3D_EXT_DIR}/lib
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
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSG_ROOT]/lib
    /usr/freeware/lib64
)

ENDMACRO(FIND_LIBNOTIFYMM_LIBRARY MYLIBRARY MYLIBRARYNAME)

SET(LIBRARYFIND_LIST notifymm notifymm-1.0)
FIND_LIBNOTIFYMM_LIBRARY(LIBNOTIFYMM_LIBRARY "${LIBRARYFIND_LIST}")

SET(LIBNOTIFYMM_FOUND "NO")
IF(LIBNOTIFYMM_LIBRARY AND LIBNOTIFYMM_INCLUDE_DIR)
    SET(LIBNOTIFYMM_FOUND "YES")
ENDIF(LIBNOTIFYMM_LIBRARY AND LIBNOTIFYMM_INCLUDE_DIR)

IF(LIBNOTIFYMM_LIBRARY)
  MESSAGE(STATUS "libnotifymm was found")
ENDIF(LIBNOTIFYMM_LIBRARY)
IF(!LIBNOTIFYMM_LIBRARY)
  MESSAGE(STATUS "libnotifymm was not found")
ENDIF(!LIBNOTIFYMM_LIBRARY)

