cmake_minimum_required(VERSION 2.8.11)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${OpenCV_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${UEYE_INCLUDE_DIRS}) 
link_directories(${OpenCV_LIB_DIR} ${Qt5Widgets_LIBRARIES} ${Boost_LIBRARY_DIRS} ${UEYE_LIBRARY_DIRS})
  

set(VideoProcessorComponent_SRCS 
					InterfaceVideoProcessor.hpp
					Buffer.h
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
			Buffer.h
			commonVideoProcessorComponent.h
			VideoProcessorAccess.h 
			VideoProcessor.h
			LocalVideo.h
			DESTINATION ${CMAKE_HOME_DIRECTORY}/include)

message("output directory is: " ${LIBRARY_OUTPUT_PATH})

