CMAKE_MINIMUM_REQUIRED (VERSION 2.8)


########################################
##  Include externally defined functions

##  Set the main path and where additional CMake modules are
IF (NOT DEFINED MAIN_SRC_PATH)
  SET (MAIN_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR})
  SET (CMAKE_MODULE_PATH ${MAIN_SRC_PATH})
ENDIF (NOT DEFINED MAIN_SRC_PATH)

INCLUDE (global)
INCLUDE (add_subdirectory_once)

##  Set the top-level path if it isn't set already
IF (NOT DEFINED TOPLEVEL_PATH)
  SET (TOPLEVEL_PATH ${CMAKE_CURRENT_BINARY_DIR})
ENDIF (NOT DEFINED TOPLEVEL_PATH)


########################################
##  Set up the software

##  Project name
PROJECT (prepair C)

##  List of source files
SET (PREPAIR_SRCFILES
  ustring.c
  stem.c
  casefold.c
  prepair.c
  word.c 
  nonword.c
  fcode.c
  main-prepair.c
  wmalloc.c
)


SET (STEM_SRCFILES 
  main-stem.c 
  ustring.c
  stem.c
  casefold.c
  wmalloc.c
)


########################################
##  Create configuration file

##  Configure a header file to pass some of the CMake settings
##  to the source code
CONFIGURE_FILE (
  "${PROJECT_SOURCE_DIR}/PrePairConfig.h.in"
  "${TOPLEVEL_PATH}/PrePairConfig.h"
  )

##  Add the binary tree to the search path for include files
INCLUDE_DIRECTORIES ("${TOPLEVEL_PATH}")


########################################
##  Create the targets

##  Create the executables
ADD_EXECUTABLE (prepair ${PREPAIR_SRCFILES})
ADD_EXECUTABLE (stem ${STEM_SRCFILES})
INSTALL (TARGETS prepair DESTINATION bin)
INSTALL (TARGETS stem DESTINATION bin)


########################################
##  Set initial compilation flags

##  Set compiler flags based on global variables; No harm in passing
##  Des-Pair flags to Re-Pair
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MY_C_FLAGS}")


########################################
##  Set up for Boost

##  Set the module path to get the latest version of it
SET (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)


########################################
##  Documentation

##  Add a target to generate Doxygen documentation
# FIND_PACKAGE (Doxygen)
# IF (DOXYGEN_FOUND)
#   CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
#   IF (NOT TARGET qscores-archiver-doc)
#     ADD_CUSTOM_TARGET (qscores-archiver-doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM)
#     INSTALL (DIRECTORY ${MAIN_BINARY_PATH}/doc/qscores-archiver DESTINATION doc OPTIONAL)
#   ENDIF (NOT TARGET qscores-archiver-doc)
# ENDIF (DOXYGEN_FOUND)


############################################################
##  CTest
############################################################

# ENABLE_TESTING ()
# ADD_TEST (NAME BitIO-ShowInfo COMMAND bitio-test --help)

