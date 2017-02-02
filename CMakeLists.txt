project(VideoProcessorComponent)

# Project info
MESSAGE( STATUS "project name = ${PROJECT_NAME}" )
MESSAGE( STATUS "source dir = ${VideoProcessorComponent_SOURCE_DIR}")
MESSAGE( STATUS "binary dir = ${VideoProcessorComponent_BINARY_DIR}")

cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_CXX_STANDARD 11)

# Preset Directories to looking for finding libraries, cmakes, ...
# It looks into the defined environment variables
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}	$ENV{QT5_DIR}
											$ENV{QT5_DIR}"/lib/cmake"
											$ENV{OPENCV_DIR}
											$ENV{BOOST_DIR}
											$ENV{BOOST_DIR}"/lib64-msvc-14.0")

 # Tell CMake to run moc when necessary:
 set(CMAKE_AUTOMOC ON)
 # As moc files are generated in the binary dir, tell CMake
 # to always look for includes there:
 set(CMAKE_INCLUDE_CURRENT_DIR ON)

# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE Debug CACHE STRING
#       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
#       FORCE)
# endif(NOT CMAKE_BUILD_TYPE)

#define the path to the cmake modules
add_subdirectory(cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

# set output directories
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_HOME_DIRECTORY}/lib)

# find library packages
find_package(OpenCV REQUIRED)
find_package(uEYE REQUIRED)
find_package(Qt5 COMPONENTS Widgets Core REQUIRED)

get_target_property(QtCore_location Qt5::Core LOCATION)

IF (Qt5Widgets_FOUND)
	MESSAGE("QT5 version: " ${Qt5Widgets_VERSION})
	
	#set the Qt libraries
	SET(QT5_LIBRARIES Qt5::Widgets)
	
ENDIF()



#find the boost libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ROOT $ENV{BOOST_ROOT})

message("BOOST_ROOT: " ${BOOST_ROOT})

find_package(Boost  COMPONENTS container thread date_time system filesystem signals)

if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	message("Boost include dir: " ${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	message("Boost libraries dir: " ${Boost_LIBRARY_DIRS})
endif()
	
# add source directory
add_subdirectory(src)



 
