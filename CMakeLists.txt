PROJECT(SplineGUI)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ENABLE_LANGUAGE(CXX)

# Add local modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Required packages
FIND_PACKAGE(GoTools REQUIRED)
FIND_PACKAGE(GoTrivariate REQUIRED)
FIND_PACKAGE(GLU REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(GPM REQUIRED)

# Required libraries
SET(DEPLIBS ${GPM_LIBRARIES} ${GoTrivariate_LIBRARIES}
            ${GoTools_LIBRARIES} ${GLUT_glut_LIBRARY} ${GLU_LIBRARY} 
)

# Required include directories
SET(INCLUDES
  ${GoTools_INCLUDE_DIRS}
  ${GoTrivariate_INCLUDE_DIRS}
  ${GLUT_INCLUDE_DIR}
  ${GLU_INCLUDE_PATH}
  ${GPM_INCLUDES}
  ${PROJECT_SOURCE_DIR}/include
)

INCLUDE_DIRECTORIES(${INCLUDES})

SET(EXECUTABLE_OUTPUT_PATH bin)
SET(LIBRARY_OUTPUT_PATH lib)

# This line may be needed only on AMD 64 bit... @jbt
# 091120: Adding this because GUV-plugins will need it. (J.O.)
IF(NOT WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(NOT WIN32)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GoTools_CXX_FLAGS}")

# Enable all warnings
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Make the Apps
FILE(GLOB_RECURSE SplineLIB_SRCS src/Button.cpp
                                 src/CurveDisplay.cpp
                                 src/CurvePoint.cpp
                                 src/Camera.cpp
                                 src/DisplayObjectSet.cpp
                                 src/OrthoProjection.cpp
                                 src/PointDisplay.cpp
                                 src/SplineGUI.cpp
                                 src/SurfaceDisplay.cpp
                                 src/VolumeDisplay.cpp)
ADD_LIBRARY(SplineGUI ${SplineLIB_SRCS})
TARGET_LINK_LIBRARIES(SplineGUI ${DEPLIBS})

ADD_EXECUTABLE(SplineGUIApp src/main.cpp)
TARGET_LINK_LIBRARIES(SplineGUIApp SplineGUI ${DEPLIBS})

ADD_EXECUTABLE(bndryCodeGUI src/main_setBndryCodes.cpp)
TARGET_LINK_LIBRARIES(bndryCodeGUI SplineGUI ${DEPLIBS})

ADD_EXECUTABLE(refineGUI src/main_refine_gui.cpp)
TARGET_LINK_LIBRARIES(refineGUI SplineGUI ${DEPLIBS})

ADD_EXECUTABLE(fenrisGUI src/main_gui.cpp)
TARGET_LINK_LIBRARIES(fenrisGUI SplineGUI ${DEPLIBS})

# 'install' target
IF(WIN32)
  # TODO
ELSE(WIN32)
  # lib
  INSTALL(TARGETS SplineGUI DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

  # binaries
  INSTALL(TARGETS SplineGUIApp bndryCodeGUI DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

  # headers
  FILE(GLOB_RECURSE SplineGUI_HEADERS include/*.h)
  INSTALL(FILES ${SplineGUI_HEADERS}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SplineGUI)

  FILE(GLOB_RECURSE LOCAL_CMAKE_MODULES cmake/Modules/*.cmake)

  # cmake modules
  INSTALL(FILES ${LOCAL_CMAKE_MODULES} DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake/Modules)
ENDIF(WIN32)
