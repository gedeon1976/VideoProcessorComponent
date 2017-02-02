# - Find uEye IDS camera libraries 
#   uEye is an USB3 industrial camera from IDS
#   here the cameras will be used to visualize the teleoperation
#
# This module defines the following variables
#  UEYE_FOUND         - system has uEye IDS 
#  UEYE_INCLUDE_DIRS  - where the uEye IDS include directory can be found
#  UEYE_LIBRARY_DIRS  - Link to this to use uEye IDS libraries
#
#  Based on cmake files from Kitware Inc

IF (WIN32)
  # the path to the libraries was predefined in the below directory
  # you can download this library from IDS signing up an account
  
  find_path(UEYE_INCLUDE_DIRS ueye.h
      "/Program Files/IDS/uEye/Develop/include")
  find_library(UEYE_LIBRARY_DIRS uEye_api_64 uEye_tools_64
      "/Program Files/IDS/uEye/Develop/lib/")

ENDIF (WIN32)

IF (UNIX)

  find_path(UEYE_INCLUDE_DIRS ueye.h
      "/usr/include")
  find_library(UEYE_LIBRARY_DIRS uEye_api uEye_tools
      "/usr/lib/")

ENDIF (UNIX)

# handle the QUIETLY and REQUIRED arguments and set UEYE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(uEYE DEFAULT_MSG UEYE_LIBRARY_DIRS UEYE_INCLUDE_DIRS)

MARK_AS_ADVANCED(UEYE_LIBRARY_DIRS UEYE_INCLUDE_DIRS )