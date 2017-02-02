cmake_minimum_required(VERSION 2.8.11)

# include directories
if (UNIX)
  include_directories(${CMAKE_CURRENT_BINARY_DIR} ${OpenCV_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${3DPOINTER_INCLUDE_DIR} ${UEYE_INCLUDE_DIRS} ) 
  
else (UNIX)
  include_directories(${CMAKE_CURRENT_BINARY_DIR} ${OpenCV_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}  ${Boost_INCLUDE_DIRS} ${3DPOINTER_INCLUDE_DIR} ${UEYE_INCLUDE_DIRS} ) 
 
endif (UNIX)

# set source code
set(testVideoProcessorComponent_SRCS 
					testVideoProcessorComponent.cpp)

# generate executable file
add_executable(testVideoProcessorComponent ${testVideoProcessorComponent_SRCS})

if (UNIX) 
  target_link_libraries(testVideoProcessorComponent ${OpenCV_LIBS} ${Qt5Widgets_LIBRARIES} ${Boost_LIBRARIES} ${3DPOINTER_LIBRARY} ${UEYE_LIBRARY_DIRS})
endif (UNIX)
if (WIN32)
  target_link_libraries(testVideoProcessorComponent ${OpenCV_LIBS} ${Qt5Widgets_LIBRARIES} ${Boost_LIBRARIES} ${3DPOINTER_LIBRARY} ${UEYE_LIBRARY_DIRS})
endif (WIN32)