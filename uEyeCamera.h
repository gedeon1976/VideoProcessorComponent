/* \brief

This is a class for access an IDS uEye Camera USB3 Camera
in our case the camera is a UI-3241LE-C-HQ 1280x1024 59fps
This is though for testing purposes or as a tool
for local teleoperation testbeds.
*/

/*
PROJECT:	3DPOINTER
COMPONENT:	VIDEO PROCESSOR
DATE:		30.10.15
AUTHOR:		HENRY PORTILLA
SPECIAL
THANKS:		GOD
*/

#pragma once

#include "commonVideoProcessorComponent.h"
#include <functional>
#include <thread>
#include <mutex>

// include IDS uEye USB3 Camera
#include "uEye.h"


// include circular buffer to be used like a reconfigurable fixed size image buffer
#include <boost\circular_buffer.hpp>
//#include <boost\circular_buffer\space_optimized.hpp>

const int DEFAULT_BUFFER_SIZE = 10;

class uEyeCamera
{
public:
    uEyeCamera();
    uEyeCamera(const uEyeCamera&);
    ~uEyeCamera();


    void startCamera();
    void selectDisplayMode();
    void adjustParameters();

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
	void setupCamera(int &camID, double width, double height);

	/// set the buffer size to save the images
	/// @param[in] bufferSize size of the circular buffer
	void setBufferSize(int bufferSize);

	/// write the received image from the camera to the buffer
	void writeToBuffer(void);

	/// get the last image from the camera image buffer
	/// @param[in,out] cameraImage save the last available image	
	void getImage(capturedFrame &cameraImage);

	/// get the next image from the camera
	cv::Mat nextImage(void);

	/// set the desired frame rate for the camera
	/// @param[in,out] fps seth the frame per second for this camera, here there will be camera real capacities
	void setFrameRate(double &fps);

	/// stop the camera
	void stopCamera();


private:

	cv::VideoCapture inputCapture;
								///< this is the local camera
	int cameraID;				///< camera identification
	double frameRate;			///< set the current frame per seconds for this camera
	int width, height;			///< width and heigh of the camera image
	std::string cameraModel;	///< save the camera model
	int bufferSize;				///< buffer size 
	boost::circular_buffer_space_optimized<capturedFrame> imageBuffer;
								///< image buffer
		
	std::mutex bufferMutex;		///< mutex to share the buffer access
	bool stopCameraThread;		///< boolean to stop the thread, C++11 doesn`t have thread cancelling


};

