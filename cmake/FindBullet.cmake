include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Bullet_PKGCONF bullet)

# Include dir
find_path(Bullet_INCLUDE_DIR
  NAMES Bullet-C-Api.h
  PATHS ${Bullet_PKGCONF_INCLUDE_DIRS} ${Bullet_INCLUDEDIR}
)

# Finally the library itself
find_library(Bullet_LIBRARY
  NAMES bulletcollision bulletdynamics bulletmath
  PATHS ${Bullet_PKGCONF_LIBRARY_DIRS} ${Bullet_LIBRARYDIR}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Bullet_PROCESS_INCLUDES Bullet_INCLUDE_DIR)
set(Bullet_PROCESS_LIBS Bullet_LIBRARY)
libfind_process(Bullet)

