# - Find BULLET
# Find the native BULLET headers and libraries.
#
#  BULLET_INCLUDE_DIR -  where to find ode.h, etc.
#  BULLET_LIBRARIES    - List of libraries when using BULLET.
#  BULLET_FOUND        - True if BULLET found.

GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

# Look for the header file.
FIND_PATH( BULLET_INCLUDE_DIR NAMES btBulletCollisionCommon.h
           PATHS /usr/local/include
                 $ENV{H3D_EXTERNAL_ROOT}/include  
                 $ENV{H3D_ROOT}/../External/include  
                 ../External/include
                 ${module_file_path}/../../External/include
                 ${module_file_path}/../../../External/include )

MARK_AS_ADVANCED(BULLET_INCLUDE_DIR)

# Look for the library.
IF(WIN32)
  FIND_LIBRARY( BULLET_COLLISION_LIBRARY NAMES libbulletcollision
                PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                      $ENV{H3D_ROOT}/../External/lib
                      ../../External/lib
                      ${module_file_path}/../../External/lib
                      ${module_file_path}/../../../External/lib )
  FIND_LIBRARY( BULLET_DYNAMICS_LIBRARY NAMES libbulletdynamics
                PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                      $ENV{H3D_ROOT}/../External/lib
                      ../../External/lib
                      ${module_file_path}/../../External/lib
                      ${module_file_path}/../../../External/lib )
  FIND_LIBRARY( BULLET_MATH_LIBRARY NAMES libbulletmath
                PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                      $ENV{H3D_ROOT}/../External/lib
                      ../../External/lib
                      ${module_file_path}/../../External/lib
                      ${module_file_path}/../../../External/lib )

ELSE(WIN32)
  FIND_LIBRARY( BULLET_COLLISION_LIBRARY NAMES BulletCollision )
  FIND_LIBRARY( BULLET_DYNAMICS_LIBRARY NAMES BulletDynamics )
  FIND_LIBRARY( BULLET_MATH_LIBRARY NAMES LinearMath )
ENDIF(WIN32)

MARK_AS_ADVANCED(BULLET_COLLISION_LIBRARY)
MARK_AS_ADVANCED(BULLET_DYNAMICS_LIBRARY)
MARK_AS_ADVANCED(BULLET_MATH_LIBRARY)

# Copy the results to the output variables.
IF(BULLET_INCLUDE_DIR AND BULLET_COLLISION_LIBRARY AND BULLET_DYNAMICS_LIBRARY AND BULLET_MATH_LIBRARY )
  SET(BULLET_FOUND 1)
  SET(BULLET_LIBRARIES ${BULLET_COLLISION_LIBRARY} ${BULLET_DYNAMICS_LIBRARY} ${BULLET_MATH_LIBRARY} )
  SET(BULLET_INCLUDE_DIR ${BULLET_INCLUDE_DIR})
ELSE(BULLET_INCLUDE_DIR AND BULLET_COLLISION_LIBRARY AND BULLET_DYNAMICS_LIBRARY AND BULLET_MATH_LIBRARY )
  SET(BULLET_FOUND 0)
  SET(BULLET_LIBRARIES)
  SET(BULLET_INCLUDE_DIR)
ENDIF(BULLET_INCLUDE_DIR AND BULLET_COLLISION_LIBRARY AND BULLET_DYNAMICS_LIBRARY AND BULLET_MATH_LIBRARY )

# Report the results.
IF(NOT BULLET_FOUND)
  SET(BULLET_DIR_MESSAGE
    "BULLET was not found. Make sure BULLET_LIBRARY and BULLET_INCLUDE_DIR are set.")
  IF(BULLET_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${BULLET_DIR_MESSAGE}")
  ELSEIF(NOT BULLET_FIND_QUIETLY)
    MESSAGE(STATUS "${BULLET_DIR_MESSAGE}")
  ENDIF(BULLET_FIND_REQUIRED)
ENDIF(NOT BULLET_FOUND)

