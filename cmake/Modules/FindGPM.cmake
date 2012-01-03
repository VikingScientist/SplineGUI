IF(GPM_INCLUDES AND GPM_LIBRARIES)
  SET(GPM_FIND_QUIETLY TRUE)
ENDIF(GPM_INCLUDES AND GPM_LIBRARIES)

FIND_PATH(GPM_INCLUDES
          NAMES
          GPM/primitives.h
          PATHS
          $ENV{HOME}/include)

FIND_LIBRARY(GPM_LIBRARIES 
             GPM
             PATHS
             $ENV{HOME}/lib)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GPM DEFAULT_MSG
                                  GPM_INCLUDES GPM_LIBRARIES)

MARK_AS_ADVANCED(GPM_INCLUDES GPM_LIBRARIES)
