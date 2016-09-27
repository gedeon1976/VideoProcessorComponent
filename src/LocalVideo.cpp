#include "LocalVideo.h"


LocalVideo::LocalVideo()
{
	// set the size of the circular buffer
	imageBuffer.set_capacity(10);
	stopCameraThread = false;	

	// image default values
	width = 1280;
	height = 1024;

	// uEye default values
	cam_ID = 0;
	m_nBitsPerPixel = 8;
	cameraBuffer = NULL;
	cameraBuffer_Id = 0;
}

// Add the copy constructor to avoid copy the std::mutex that is not copyable
LocalVideo::LocalVideo(const LocalVideo&)
{
	// set the size of the circular buffer
	imageBuffer.set_capacity(10);
	stopCameraThread = false;

	// image default values
	width = 1280;
	height = 1024;

	// uEye default values
	cam_ID = 0;
	cameraBuffer = NULL;
	cameraBuffer_Id = 0;
}

LocalVideo::~LocalVideo()
{
	if (cameraBuffer != NULL )
		is_FreeImageMem(cam_ID, cameraBuffer, cameraBuffer_Id);
	
	cameraBuffer = NULL;

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

	INT nRet;
	cv::Mat currentImage;	
	
	is_FreezeVideo(cam_ID, IS_DONT_WAIT);
	DWORD dwRet = WaitForSingleObject(hEvent, 1000);
	//is_GetImageMem(cam_ID, (void**)cameraBuffer);	
	if (dwRet == WAIT_TIMEOUT){

	}
	else if (dwRet == WAIT_OBJECT_0){
		frame->imageData = (char*)cameraBuffer;
		currentImage = cv::cvarrToMat(frame);
	}	

	return currentImage;
}

// set the frame rate for the camera
void LocalVideo::setFrameRate(double &fps){

	double newFps;
	frameRate = fps;
	is_SetFrameRate(cam_ID, frameRate, &newFps);

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
	cam_ID = hCamID;
	nRet = is_InitCamera(&cam_ID, m_hWndDisplay);// no handle window, DIB mode save to RAM memory
												   
	if (nRet == IS_SUCCESS)
	{
		// Get sensor info
		is_GetSensorInfo(cam_ID, &m_sInfo);

		setWidth(m_sInfo.nMaxWidth);
		setHeight(m_sInfo.nMaxHeight);

		GetMaxImageSize(&m_nSizeX, &m_nSizeY);

		nRet = InitCamera();
		nRet = InitDisplayMode();

		//led to green 
		is_IO(cam_ID, IS_IO_CMD_LED_SET_STATE, (void*)&nCurrentState, sizeof(nCurrentState));

		// start live video
		is_CaptureVideo(cam_ID, IS_WAIT);		
	}
	else{

		return nRet;
	}
		

	//	if (nRet == IS_SUCCESS)
	//	{
	//		// Enable Messages
	//		is_EnableMessage(cam_ID, IS_DEVICE_REMOVED, GetSafeHwnd());
	//		is_EnableMessage(cam_ID, IS_DEVICE_RECONNECTED, GetSafeHwnd());
	//		is_EnableMessage(cam_ID, IS_FRAME, GetSafeHwnd());


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

INT LocalVideo::InitCamera(){

	INT nRet;
	int width = getWidth();
	int height = getHeight();
	int nChannels = 3;
	//m_nBitsPerPixel = 8;

	// IplImage to save image from the uEye camera
	frame = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, nChannels);

	// Init uEye memory for the IDS driver
	nRet = is_AllocImageMem(cam_ID, width, height, m_nBitsPerPixel, &cameraBuffer, &cameraBuffer_Id);

	// active the memory to be used as a image frame buffer by the IDS driver
	nRet = is_SetImageMem(cam_ID, cameraBuffer, cameraBuffer_Id);
	
	// setup IplImage
	frame->ID = 0;
	frame->nChannels = nChannels;
	frame->alphaChannel = 0;
	frame->depth = IPL_DEPTH_8U;
	frame->dataOrder = 0;
	frame->origin = 0;

	frame->align = 1;
	frame->width = width;
	frame->height = height;
	frame->roi = NULL;
	frame->maskROI = NULL;
	frame->imageId = NULL;
	frame->tileInfo = NULL;
	frame->imageSize = nChannels*width*height;
	frame->widthStep = 4*width;

	frame->imageDataOrigin = (char*)cameraBuffer;
	frame->imageData = (char*)cameraBuffer;	

	// Init frame events windows version
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	nRet = is_InitEvent(cam_ID, hEvent, IS_SET_EVENT_FRAME);
	nRet = is_EnableEvent(cam_ID, IS_DONT_WAIT);

	return nRet;
}

void LocalVideo::ExitCamera(){

	int nCurrentState = IO_LED_STATE_1;

	if (cam_ID != 0)
	{
		// Disable messages
		is_EnableMessage(cam_ID, IS_FRAME, NULL);

		// Stop live video
		is_StopLiveVideo(cam_ID, IS_WAIT);
		
		//led to orange
		is_IO(cam_ID, IS_IO_CMD_LED_SET_STATE, (void*)&nCurrentState, sizeof(nCurrentState));

		// Free the allocated buffer
		if (cameraBuffer != NULL)
			is_FreeImageMem(cam_ID, cameraBuffer, cameraBuffer_Id);

		cameraBuffer = NULL;

		// Close camera
		is_ExitCamera(cam_ID);
		cam_ID = NULL;
	}

}

// init the display mode
int  LocalVideo::InitDisplayMode(){

	INT nRet = IS_NO_SUCCESS;

	if (cam_ID == NULL)
		return IS_NO_SUCCESS;

	//if (cameraBuffer != NULL)
	//{
	//	is_FreeImageMem(cam_ID, cameraBuffer, cameraBuffer_Id);// Free the memory 
	//}
	//cameraBuffer = NULL;

	// Set display mode to DIB
	nRet = is_SetDisplayMode(cam_ID, IS_SET_DM_DIB);
	if (m_sInfo.nColorMode == IS_COLORMODE_BAYER)
	{
		// setup the color depth to the current windows setting
		is_GetColorDepth(cam_ID, &m_nBitsPerPixel, &m_nColorMode);
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

	
	//active the memory to save the captured image
	nRet = is_SetImageMem(cam_ID, cameraBuffer, cameraBuffer_Id);
	

	if (nRet == IS_SUCCESS)
	{
		// set the desired color mode, here DIB,
		is_SetColorMode(cam_ID, m_nColorMode);

		// set the image size to capture
		IS_SIZE_2D imageSize;
		imageSize.s32Width = m_nSizeX;
		imageSize.s32Height = m_nSizeY;

		is_AOI(cam_ID, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));
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
	if (is_ImageFormat(cam_ID,
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
		is_GetSensorInfo(cam_ID, &sInfo);
		*pnSizeX = sInfo.nMaxWidth;
		*pnSizeY = sInfo.nMaxHeight;
	}
	else
	{
		// Only ueye xs
		// Get image size of the current format
		IS_SIZE_2D imageSize;
		is_AOI(cam_ID, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

		*pnSizeX = imageSize.s32Width;
		*pnSizeY = imageSize.s32Height;
	}

}

// set the width of the image
void LocalVideo::setWidth(INT imageWidth){

	width = imageWidth;
}

// set the height of the image
void LocalVideo::setHeight(INT imageHeight){

	height = imageHeight;
}

// get the width of the image
INT LocalVideo::getWidth(){

	INT currentWidth = width;
	return currentWidth;
}

// get the height of the image
INT LocalVideo::getHeight(){

	INT currentHeight = height;
	return currentHeight;
}