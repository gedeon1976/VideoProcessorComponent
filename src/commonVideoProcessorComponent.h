
/*! \brief

This header contain common definitions used in the
VideoProcessor Component

*/

#pragma once

// include OpenCV 
#include <opencv2\core.hpp>
#include <opencv2\calib3d.hpp>
#include <opencv2\videoio.hpp>


// include c++ chrono
#include <chrono>


/// this structure save a captured image and the time of capturing
struct capturedFrame
{
	cv::Mat image;
	std::chrono::time_point<std::chrono::system_clock> timeofCapture;
	
};


