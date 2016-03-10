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
#include <functional>
#include <thread>
#include <mutex>


// include circular buffer to be used like a reconfigurable fixed size image buffer
#include <boost\circular_buffer.hpp>
//#include <boost\circular_buffer\space_optimized.hpp>

const int DEFAULT_BUFFER_SIZE = 10;

class LocalVideo
{
public:
	LocalVideo();
	LocalVideo(const LocalVideo&);
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
	/// @param[in,out] fps set the frame per second for this camera
	void setFrameRate(double &fps);

	/// stop the camera
	void stopCamera();

private:

	// uEye methods
	INT OpenCamera(HIDS hCamID);
	void ExitCamera();
	int  InitDisplayMode();
	void LoadParameters();
	void GetMaxImageSize(INT *pnSizeX, INT *pnSizeY);

	HICON	m_hIcon;

	// uEye variables
	HIDS	m_hCamID;			// cameraID
	HWND	m_hWndDisplay;		// handle to display window
	INT		m_nColorMode;		// Y8/RGB16/RGB24/REG32
	INT		m_nBitsPerPixel;	// number of bits needed store one pixel
	INT		m_nSizeX;			// width of image
	INT		m_nSizeY;			// height of image
	INT		m_nPosX;			// left offset of image
	INT		m_nPosY;			// right offset of image

	// memory needed for live display while using DIB
	INT		m_lMemoryId;		// camera memory - buffer ID
	char*	m_pcImageMemory;	// camera memory - pointer to buffer
	SENSORINFO m_sInfo;			// sensor information struct
	INT     m_nRenderMode;		// render  mode
	INT     m_nFlipHor;			// horizontal flip flag
	INT     m_nFlipVert;		// vertical flip flag

	IplImage *frame;			// save the frame from uEye

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

