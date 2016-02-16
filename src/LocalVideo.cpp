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

// write to the buffer
void LocalVideo::writeToBuffer(void){

	
	// open the camera
	inputCapture.open(cameraID);
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
	if (inputCapture.isOpened()) {
		cv::Mat tmpImage;
		inputCapture >> tmpImage;
		tmpImage.copyTo(currentImage);
	}
	
	return currentImage;
}

// set the frame rate for the camera
void LocalVideo::setFrameRate(double &fps){

	frameRate = fps;

}

// stop the camera 
void LocalVideo::stopCamera(){

	stopCameraThread = true;

}


// PRIVATE METHODS

INT LocalVideo::OpenCamera(HIDS *hCam, HWND hWnd){

	INT nRet = IS_NO_SUCCESS;
	ExitCamera();

	// init camera (open next available camera)
	m_hCam = (HIDS)0;
	nRet = is_InitCamera(&m_hCam, m_hWndDisplay);
	if (nRet == IS_SUCCESS)
	{
		// Get sensor info
		is_GetSensorInfo(m_hCam, &m_sInfo);

		GetMaxImageSize(&m_nSizeX, &m_nSizeY);

		nRet = InitDisplayMode();
		
	}
	return nRet;
	//	if (nRet == IS_SUCCESS)
	//	{
	//		// Enable Messages
	//		is_EnableMessage(m_hCam, IS_DEVICE_REMOVED, GetSafeHwnd());
	//		is_EnableMessage(m_hCam, IS_DEVICE_RECONNECTED, GetSafeHwnd());
	//		is_EnableMessage(m_hCam, IS_FRAME, GetSafeHwnd());

	//		// start live video
	//		is_CaptureVideo(m_hCam, IS_WAIT);

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

	if (m_hCam != 0)
	{
		// Disable messages
		is_EnableMessage(m_hCam, IS_FRAME, NULL);

		// Stop live video
		is_StopLiveVideo(m_hCam, IS_WAIT);

		// Free the allocated buffer
		if (m_pcImageMemory != NULL)
			is_FreeImageMem(m_hCam, m_pcImageMemory, m_lMemoryId);

		m_pcImageMemory = NULL;

		// Close camera
		is_ExitCamera(m_hCam);
		m_hCam = NULL;
	}

}
int  LocalVideo::InitDisplayMode(){

	INT nRet = IS_NO_SUCCESS;

	if (m_hCam == NULL)
		return IS_NO_SUCCESS;

	if (m_pcImageMemory != NULL)
	{
		is_FreeImageMem(m_hCam, m_pcImageMemory, m_lMemoryId);
	}
	m_pcImageMemory = NULL;

	// Set display mode to DIB
	nRet = is_SetDisplayMode(m_hCam, IS_SET_DM_DIB);
	if (m_sInfo.nColorMode == IS_COLORMODE_BAYER)
	{
		// setup the color depth to the current windows setting
		is_GetColorDepth(m_hCam, &m_nBitsPerPixel, &m_nColorMode);
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

	// allocate an image memory.
	if (is_AllocImageMem(m_hCam, m_nSizeX, m_nSizeY, m_nBitsPerPixel, &m_pcImageMemory, &m_lMemoryId) != IS_SUCCESS)
	{
		//AfxMessageBox(TEXT("Memory allocation failed!"), MB_ICONWARNING);
	}
	else
		is_SetImageMem(m_hCam, m_pcImageMemory, m_lMemoryId);


	if (nRet == IS_SUCCESS)
	{
		// set the desired color mode
		is_SetColorMode(m_hCam, m_nColorMode);

		// set the image size to capture
		IS_SIZE_2D imageSize;
		imageSize.s32Width = m_nSizeX;
		imageSize.s32Height = m_nSizeY;

		is_AOI(m_hCam, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));
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
	if (is_ImageFormat(m_hCam,
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
		is_GetSensorInfo(m_hCam, &sInfo);
		*pnSizeX = sInfo.nMaxWidth;
		*pnSizeY = sInfo.nMaxHeight;
	}
	else
	{
		// Only ueye xs
		// Get image size of the current format
		IS_SIZE_2D imageSize;
		is_AOI(m_hCam, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

		*pnSizeX = imageSize.s32Width;
		*pnSizeY = imageSize.s32Height;
	}

}