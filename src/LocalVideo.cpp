#include "LocalVideo.h"


LocalVideo::LocalVideo()
{
	// set the size of the circular buffer
	imageBuffer.set_capacity(10);
	stopCameraThread = false;	
}

// Add the copy constructor to avoid copy the std::mutex that is not copyable
LocalVideo::LocalVideo(const LocalVideo&)
{
	// set the size of the circular buffer
	imageBuffer.set_capacity(10);
	stopCameraThread = false;
}

LocalVideo::~LocalVideo()
{
}

// set the camera name
void LocalVideo::setCameraName(std::string &cameraName){

	cameraModel = cameraName;

}

// get the camera name
void LocalVideo::getCameraName(std::string &cameraName){

	cameraName = cameraModel;
}

// connect to the camera and start the image grabbing
void LocalVideo::setupCamera(int &camID, double width, double height){

	bool isCameraOk;
	// setup the camera
	//inputCapture.set(cv::CAP_PROP_FRAME_WIDTH, width);
	//inputCapture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
	//inputCapture.set(cv::CAP_PROP_FPS, frameRate);
	cameraID = camID;

	// open the uEye Camera  
	OpenCamera(cameraID);

	////isCameraOk = inputCapture.open(cameraID);
	//if (isCameraOk)
	//{
	//	cv::Mat tmpFrame;
	//	inputCapture >> tmpFrame;
	//}
		
}


// set the buffer size
void LocalVideo::setBufferSize(int bufferSize){

	imageBuffer.set_capacity(bufferSize);

}

// write to the circular buffer,the old values will be overwritten
void LocalVideo::writeToBuffer(void){

	capturedFrame actualCapturedFrame;
	cv::Mat currentFrame;
	std::chrono::time_point<std::chrono::system_clock> currentCapturedTime;	

	/// thread loop
	while (!stopCameraThread){
	
		// get the current image
		currentFrame = nextImage();

		// save the captured frame
		currentFrame.copyTo(actualCapturedFrame.image);
		actualCapturedFrame.timeofCapture = std::chrono::system_clock::now();
		
		// write to the buffer using a lock guard mutex to keep a correct memory management
		bufferMutex.lock();
		imageBuffer.push_back(actualCapturedFrame);
		bufferMutex.unlock();	

	}
}

// get the last available image
void LocalVideo::getImage(capturedFrame &imageCamera){

	capturedFrame actualFrame;

	// read from the buffer using a lock guard mutex to keep a correct memory management
	std::lock_guard<std::mutex> lock(bufferMutex);
	//bufferMutex.lock();
	actualFrame = imageBuffer.front();		// get the last image from circular buffer
	imageCamera = actualFrame;
	//bufferMutex.unlock();
}

// get the next image
cv::Mat LocalVideo::nextImage(void){

	cv::Mat currentImage;
	IplImage *img = cvCreateImageHeader(cvSize(m_nSizeX, m_nSizeY), IPL_DEPTH_8U, 3);

	is_FreezeVideo(m_hCamID, IS_DONT_WAIT);

	img->imageData = (char*)m_pcImageMemory;
	currentImage = cv::cvarrToMat(img);

	return currentImage;
}

// set the frame rate for the camera
void LocalVideo::setFrameRate(double &fps){

	double newFps;
	frameRate = fps;
	is_SetFrameRate(m_hCamID, frameRate, &newFps);

}

// stop the camera 
void LocalVideo::stopCamera(){

	stopCameraThread = true;

}


// PRIVATE METHODS

INT LocalVideo::OpenCamera(HIDS hCamID){

	INT nRet = IS_NO_SUCCESS;
	int nCurrentState = IO_LED_STATE_2;
	
	// init camera 
	m_hCamID = hCamID;
	nRet = is_InitCamera(&m_hCamID, m_hWndDisplay);// no handle window, DIB mode save to RAM memory
												   

	if (nRet == IS_SUCCESS)
	{
		// Get sensor info
		is_GetSensorInfo(m_hCamID, &m_sInfo);

		GetMaxImageSize(&m_nSizeX, &m_nSizeY);

		nRet = InitDisplayMode();
		
	}
		
	//led to green 
	is_IO(m_hCamID, IS_IO_CMD_LED_SET_STATE, (void*)&nCurrentState, sizeof(nCurrentState));

	// start live video
	is_CaptureVideo(m_hCamID, IS_WAIT);

	return nRet;
	//	if (nRet == IS_SUCCESS)
	//	{
	//		// Enable Messages
	//		is_EnableMessage(m_hCamID, IS_DEVICE_REMOVED, GetSafeHwnd());
	//		is_EnableMessage(m_hCamID, IS_DEVICE_RECONNECTED, GetSafeHwnd());
	//		is_EnableMessage(m_hCamID, IS_FRAME, GetSafeHwnd());


	//	}
	//	else
	//		//AfxMessageBox(TEXT("Initializing the display mode failed!"), MB_ICONWARNING);

	//	return true;
	//}
	//else
	//{
	//	//AfxMessageBox(TEXT("No uEye camera could be opened !"), MB_ICONWARNING);
	//	return false;
	//}


}

void LocalVideo::ExitCamera(){

	int nCurrentState = IO_LED_STATE_1;

	if (m_hCamID != 0)
	{
		// Disable messages
		is_EnableMessage(m_hCamID, IS_FRAME, NULL);

		// Stop live video
		is_StopLiveVideo(m_hCamID, IS_WAIT);
		
		//led to orange
		is_IO(m_hCamID, IS_IO_CMD_LED_SET_STATE, (void*)&nCurrentState, sizeof(nCurrentState));

		// Free the allocated buffer
		if (m_pcImageMemory != NULL)
			is_FreeImageMem(m_hCamID, m_pcImageMemory, m_lMemoryId);

		m_pcImageMemory = NULL;

		// Close camera
		is_ExitCamera(m_hCamID);
		m_hCamID = NULL;
	}

}

// init the display mode
int  LocalVideo::InitDisplayMode(){

	INT nRet = IS_NO_SUCCESS;

	if (m_hCamID == NULL)
		return IS_NO_SUCCESS;

	if (m_pcImageMemory != NULL)
	{
		is_FreeImageMem(m_hCamID, m_pcImageMemory, m_lMemoryId);// Free the memory 
	}
	m_pcImageMemory = NULL;

	// Set display mode to DIB
	nRet = is_SetDisplayMode(m_hCamID, IS_SET_DM_DIB);
	if (m_sInfo.nColorMode == IS_COLORMODE_BAYER)
	{
		// setup the color depth to the current windows setting
		is_GetColorDepth(m_hCamID, &m_nBitsPerPixel, &m_nColorMode);
	}
	else if (m_sInfo.nColorMode == IS_COLORMODE_CBYCRY)
	{
		// for color camera models use RGB32 mode
		m_nColorMode = IS_CM_BGRA8_PACKED;
		m_nBitsPerPixel = 32;
	}
	else
	{
		// for monochrome camera models use Y8 mode
		m_nColorMode = IS_CM_MONO8;
		m_nBitsPerPixel = 8;
	}

	
	if (is_AllocImageMem(m_hCamID, m_nSizeX, m_nSizeY, m_nBitsPerPixel, &m_pcImageMemory, &m_lMemoryId) != IS_SUCCESS)
	{
	//	//AfxMessageBox(TEXT("Memory allocation failed!"), MB_ICONWARNING);
	//	// set a message here
	}
	else
		// active the memory to save the captured image
		is_SetImageMem(m_hCamID, m_pcImageMemory, m_lMemoryId);


	if (nRet == IS_SUCCESS)
	{
		// set the desired color mode, here DIB,
		is_SetColorMode(m_hCamID, m_nColorMode);

		// set the image size to capture
		IS_SIZE_2D imageSize;
		imageSize.s32Width = m_nSizeX;
		imageSize.s32Height = m_nSizeY;

		is_AOI(m_hCamID, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));
	}

	return nRet;

}
// Load parameters from the camera
void LocalVideo::LoadParameters(){

}
void LocalVideo::GetMaxImageSize(INT *pnSizeX, INT *pnSizeY){

	// Check if the camera supports an arbitrary AOI
	// Only the ueye xs does not support an arbitrary AOI
	INT nAOISupported = 0;
	BOOL bAOISupported = TRUE;
	if (is_ImageFormat(m_hCamID,
		IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED,
		(void*)&nAOISupported,
		sizeof(nAOISupported)) == IS_SUCCESS)
	{
		bAOISupported = (nAOISupported != 0);
	}

	if (bAOISupported)
	{
		// All other sensors
		// Get maximum image size
		SENSORINFO sInfo;
		is_GetSensorInfo(m_hCamID, &sInfo);
		*pnSizeX = sInfo.nMaxWidth;
		*pnSizeY = sInfo.nMaxHeight;
	}
	else
	{
		// Only ueye xs
		// Get image size of the current format
		IS_SIZE_2D imageSize;
		is_AOI(m_hCamID, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

		*pnSizeX = imageSize.s32Width;
		*pnSizeY = imageSize.s32Height;
	}

}