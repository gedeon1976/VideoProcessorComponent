/* \brief

This is a class for access local video from a USB camera
This is though for testing purposes or as a tool
for local teleoperation testbeds.
*/

/*
PROJECT:	3DPOINTER
COMPONENT:	VIDEO PROCESSOR
DATE:		27.05.15
AUTHOR:		HENRY PORTILLA
SPECIAL
THANKS:		GOD
*/

#pragma once

#include "commonVideoProcessorComponent.h"
#include <thread>
#include <mutex>


// include circular buffer to be used like a reconfigurable fixed size image buffer
#include <boost\circular_buffer.hpp>
#include <boost\circular_buffer\space_optimized.hpp>

const int DEFAULT_BUFFER_SIZE = 10;

class LocalVideo
{
public:
	LocalVideo();
	~LocalVideo();

	/// set the camera name
	/// @param[in,out] cameraName name given to this camera
	void setCameraName(std::string &cameraName);

	/// get the camera name
	/// @param[in,out] cameraName name given to this camera
	void getCameraName(std::string &cameraName);

	/// connect to the camera and start a thread for recovery images
	/// @param[in,out] cameraID is the ID for this camera
	/// @param[in] width image width for this camera
	/// @param[in] height image height for this camera
	void startCamera(int &cameraID, double width, double height);


	/// set the buffer size to save the images
	/// @param[in] bufferSize size of the circular buffer
	void setBufferSize(int bufferSize);

	/// write the received image from the camera to the buffer
	void writeToBuffer();

	/// get the last image from the camera image buffer
	/// @param[in,out] cameraImage save the last available image	
	void getImage(cv::Mat &cameraImage);

	/// set the desired frame rate for the camera
	/// @param[in,out] fps seth the frame per second for this camera, here there will be camera real capacities
	void setFrameRate(int &fps);

	/// stop the camera
	void stopCamera();


private:

	cv::VideoCapture localCamera;
								///< this is the local camera
	int cameraID;				///< camera identification
	double frameRate;			///< set the current frame per seconds for this camera
	int width, height;			///< width and heigh of the camera image
	std::string cameraModel;	///< save the camera model
	int bufferSize;				///< buffer size 
	boost::circular_buffer_space_optimized<capturedFrame> imageBuffer;
								///< image buffer

	std::thread cameraThread;	///< local camera thread
	std::thread::id threadID;	///< ID for the thread that write to the buffer
	std::mutex bufferMutex;		///< mutex to share the cuffer access
	bool stopCameraThread;		///< boolean to stop the thread, C++11 doesn`t have thread canceling


};

