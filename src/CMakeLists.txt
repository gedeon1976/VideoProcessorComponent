cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${UEYE_INCLUDE_DIRS}) 
link_directories(${OpenCV_LIB_DIR} ${Boost_LIBRARY_DIRS} ${UEYE_LIBRARY_DIRS})
  

set(VideoProcessorComponent_SRCS 
					InterfaceVideoProcessor.hpp
					LocalVideo.h
					VideoProcessorAccess.h
					commonVideoProcessorComponent.h		
					VideoProcessor.h 
					LocalVideo.cpp
					VideoProcessorAccess.cpp
					VideoProcessor.cpp)


add_library(VideoProcessorComponent STATIC ${VideoProcessorComponent_SRCS})
set(LIBRARY_OUTPUT_PATH ${CMAKE_HOME_DIRECTORY}/lib )

# copy the interface header to the library
file(COPY	InterfaceVideoProcessor.hpp 
			commonVideoProcessorComponent.h
			VideoProcessorAccess.h 
			VideoProcessor.h			
			DESTINATION ${CMAKE_HOME_DIRECTORY}/include)

message("output directory is: " ${LIBRARY_OUTPUT_PATH})

