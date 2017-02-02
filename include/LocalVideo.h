 

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
#include <iostream>
//#include <qobject.h> 

// include thread buffer class
#include "Buffer.h"

// include circular buffer to be used like a reconfigurable fixed size image buffer
#include <boost\circular_buffer.hpp>
#include <boost\filesystem.hpp>
//#include <boost\circular_buffer\space_optimized.hpp>

using namespace std;

const int DEFAULT_BUFFER_SIZE = 10;
const int MAX_EVENTS = 32;
const int SEQ_BUFFERS = 3;

/// structure for select the display mode
typedef enum _disp_mode
{
	e_disp_mode_bitmap = 0,
	e_disp_mode_direct3d
};

/// structure for save uEye camera Information
struct Camera
{
	HIDS		camID;
	char*		cameraBuffer;
	int			cameraBufferID;
	int			RenderMode;
	SENSORINFO	SensorInfo;
	CAMINFO		CamInfo;
	UEYE_CAMERA_INFO	uEyeInfo;
	int         nBitsPerPixel;
	bool		OnLive;
	bool        IsOpened;

	// Event uEye variables
	HANDLE		EventList[32];		// event handle array
	INT			EventType[32];		// type of uEye event
	int			EventCounter[32];	// event counter
	int			EventsChecked[32];	// GUI flag for checking the radio button
	int			MaxEvents;			// maximum number of events we use

	// image Buffer
	Buffer<capturedFrame>	imageBuffer;
	
};

class LocalVideo  //: public QObject
{
	//Q_OBJECT
public:
	LocalVideo();
	LocalVideo(LocalVideo& localVideo);
	~LocalVideo();

	/// set the camera name
	/// @param[in,out] cameraName name given to this camera
	void setCameraName(std::string &cameraName);

	/// get the camera name
	/// @param[in,out] cameraName name given to this camera
	void getCameraName(std::string &cameraName);
	INT getCameraID();

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
	void getCurrentImage(capturedFrame &cameraImage);
	void getImage(void);

	/// get the next image from the camera
	cv::Mat nextImage(void);

	/// set the desired frame rate for the camera
	/// @param[in,out] fps set the frame per second for this camera
	void setFrameRate(double &fps);

	/// stop the camera
	void stopCamera();

	/// Signal to send the captured images
//signals:
	//void sendImage(cv::Mat currentImage); 


private:

	// uEye methods
	INT OpenCamera(HIDS hCamID);
	INT InitCamera(HIDS *hCam, HWND hWnd);
	void StartCamera();
	void ExitCamera();
	int  InitDisplayMode(int modeSelection);
	bool InitializeEvents();
	void LoadParameters();
	void GetMaxImageSize(INT *pnSizeX, INT *pnSizeY);
	bool checkCamera(HIDS hCamID);
	

	// Event functions
	bool EventEnumerate();
	bool GetIndexEvent(INT nEvent, int* pnIndex);
	bool EnableEvent(INT nEvent, bool bEnable);
	bool InitEvent(INT nEvent);
	void ThreadProc();
	//bool InitAllEvents();
	bool TerminateAllEvents();

	void setWidth(INT currentWidth);
	void setHeight(INT currentHeight);

	INT getWidth();
	INT getHeight();

	// Sequence functions
	bool SeqBuilt();
	bool SeqKill();

	// Utility functions
	bool getFilePath(string &fileName, string &pathFound);


	HICON	m_hIcon;

	// uEye variables
	HIDS	cam_ID;				// cameraID
	HWND	m_hWndDisplay;		// handle to display window
	INT     m_nDispModeSel;		// Display mode
	INT		m_nColorMode;		// Y8/RGB16/RGB24/REG32
	INT		m_nBitsPerPixel;	// number of bits needed store one pixel
	INT		m_nSizeX;			// width of image
	INT		m_nSizeY;			// height of image
	INT		m_nPosX;			// left offset of image
	INT		m_nPosY;			// right offset of image
	HANDLE	hEvent;				// event to get camera images
	INT		m_Ret;				// return value of uEye SDK functions

	Camera	m_Camera;			// Camera data structure

	// memory needed for live display while using DIB
	INT		cameraBuffer_Id;	// camera memory - buffer ID
	char*	cameraBuffer;		// camera memory - pointer to buffer
	INT		m_lSeqMemId[SEQ_BUFFERS];		// camera memory - buffer ID
	char*	m_pcSeqImgMem[SEQ_BUFFERS];		// camera memory - pointer to buffer
	SENSORINFO m_sInfo;			// sensor information struct
	INT     m_nRenderMode;		// render  mode
	INT     m_nFlipHor;			// horizontal flip flag
	INT     m_nFlipVert;		// vertical flip flag

	// Event variables
	HANDLE  m_hEventList[MAX_EVENTS];		// event handle array
	INT     m_nEventType[MAX_EVENTS];		// type of uEye event
	int     m_nEventCounter[MAX_EVENTS];	// event counter
	int     m_nEventsChecked[MAX_EVENTS];	// GUI flag for checking the radio button
	int     m_MaxEvents;					// maximum number of events we use
	

	// Thread
	HANDLE  m_hThread;
	BOOL    m_boRunThread;
	DWORD   m_dwThreadID;

	IplImage *frame;			// save the frame from uEye

	cv::VideoCapture inputCapture;
								///< this is the local camera
	int cameraID;				///< camera identification
	double frameRate;			///< set the current frame per seconds for this camera
	INT width, height;		    ///< width and heigh of the camera image
	std::string cameraModel;	///< save the camera model

	

	int bufferSize;				///< buffer size 
	//boost::circular_buffer_space_optimized<capturedFrame> imageBuffer;
								///< image buffer
		
	//std::mutex bufferMutex;		///< mutex to share the buffer access
	//std::condition_variable bufferCondVar;
								///< condition variable for thread synchronization
	bool stopCameraThread;		///< boolean to stop the thread, C++11 doesn`t have thread cancelling



};

