#include "LocalVideo.h"


LocalVideo::LocalVideo()
{
	stopCameraThread = false;	

	// image default values
	width = 1280;
	height = 1024;

	// uEye default m_MaxEvents
	//cam_ID = 0;
	m_nBitsPerPixel = 8;
	cameraBuffer = NULL;
	cameraBuffer_Id = 0;
	m_MaxEvents = 11;	
}

 //Add the copy constructor to avoid copy the std::mutex that is not copyable
//LocalVideo::LocalVideo(LocalVideo& localVideo)
//{
//	stopCameraThread = false;
//
//	// image default values
//	width = 1280;
//	height = 1024;
//
//	// uEye default values
//	//cam_ID = 0;
//	cameraBuffer = NULL;
//	cameraBuffer_Id = 0;
//	m_MaxEvents = 11;	
//	INT cameraID;
//
//	// copy all the important data for this camera
//	//m_Camera = localVideo.m_Camera;
//	
//}

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
	cam_ID = camID;

	// set the size of camera buffer
	m_Camera.imageBuffer.setSize(DEFAULT_BUFFER_SIZE);

	// open the uEye Camera  
	OpenCamera(cameraID);	

	// enable the events
	//InitializeEvents();

	////isCameraOk = inputCapture.open(cameraID);
	//if (isCameraOk)
	//{
	//	cv::Mat tmpFrame;
	//	inputCapture >> tmpFrame;
	//}
		
	
}


// set the buffer size
void LocalVideo::setBufferSize(int bufferSize){

	m_Camera.imageBuffer.setSize(bufferSize);

}

// write to the circular buffer,the old values will be overwritten
void LocalVideo::writeToBuffer(void) {

	capturedFrame actualCapturedFrame;
	cv::Mat currentFrame;

	// Check Events from UEye cameras
	DWORD lReturn;
	int nEvIndex;
	DWORD static Time1, Time2;
	INT cameraID = 1;
	//cam_ID = is_SetCameraID(IS_GET_CAMERA_ID);
	// set display mode
	m_nDispModeSel = _disp_mode::e_disp_mode_bitmap;
	m_nBitsPerPixel = 24;

	// find the latest image buffer and display the buffer
	cv::Mat currentImage;
	INT nNum;
	void *pMemVoid;
	char* pMemChar;
	INT memoryID;
	char *pcMem, *pcMemLast;
	char* pPixelPointer;
	INT lineSize;

	// Get the size of the line increment
	is_GetImageMemPitch(m_Camera.camID, &lineSize);
	int adjust = 0;
	int bufferSize = lineSize*m_Camera.SensorInfo.nMaxHeight;
	pMemChar = new char[bufferSize];
	IplImage *Iframe;
	Iframe = cvCreateImageHeader(cvSize(m_Camera.SensorInfo.nMaxWidth, m_Camera.SensorInfo.nMaxHeight), IPL_DEPTH_8U, 3);

	// call initialize events
	InitializeEvents();
	//InitDisplayMode(e_disp_mode_bitmap);

	SeqBuilt();
	StartCamera();

	// Camera is OnLive
	m_Camera.OnLive = true;

	/// thread loop
	while (!stopCameraThread) {

		// wait to get signaled
		lReturn = WaitForMultipleObjects(m_MaxEvents, m_hEventList, FALSE, INFINITE);
		if (!stopCameraThread) {

			nEvIndex = lReturn - WAIT_OBJECT_0;
			// get time stamp
			m_nEventsChecked[nEvIndex] = ::GetTickCount();

			// update counter and GUI
			m_nEventCounter[nEvIndex] ++;
			//SetDlgItemInt(IDC_EDIT_EV_0 + nEvIndex, m_nEvCount[nEvIndex]);

			//CheckDlgButton(IDC_RADIO_EVENT_0 + nEvIndex, BST_CHECKED);

			// display the sequenzbuffer or in bitmap live mode
			if (m_nEventType[nEvIndex] == IS_SET_EVENT_FRAME)
			{
				// in sequence mode
				/*if (!GetDlgItem(IDC_BUTTON_SEQ_START)->IsWindowEnabled()
				& !GetDlgItem(IDC_BUTTON_START_LIVE)->IsWindowEnabled()
				& GetDlgItem(IDC_BUTTON_SEQ_STOP)->IsWindowEnabled())*/
				//{
					is_GetActSeqBuf(m_Camera.camID, &nNum, &pcMem, &pcMemLast);
					int j;
					for (j = 0; j<SEQ_BUFFERS; j++)
					{
						if (pcMemLast == m_pcSeqImgMem[j])
						{
							//std::memcpy(pMemChar, m_pcSeqImgMem[j], sizeof pMemChar );
							//is_LockSeqBuf(m_Camera.camID, m_lSeqMemId[j], m_pcSeqImgMem[j]);

							is_CopyImageMem(m_Camera.camID, m_pcSeqImgMem[j], m_lSeqMemId[j], pMemChar);

							//is_UnlockSeqBuf(m_Camera.camID, m_lSeqMemId[j], m_pcSeqImgMem[j]);

							cvSetData(Iframe, pMemChar, Iframe->widthStep);
							//currentImage = cv::cvarrToMat(frame);
							//is_GetImageMem(m_Camera.camID, &pMemVoid);							
							//pPixelPointer = reinterpret_cast<char*> (pMemVoid);
							cv::Mat currentFrame(cvSize(m_Camera.SensorInfo.nMaxWidth, m_Camera.SensorInfo.nMaxHeight), CV_8UC3, pMemChar);
							
							// save the captured frame
							currentFrame.copyTo(actualCapturedFrame.image);
							actualCapturedFrame.timeofCapture = std::chrono::system_clock::now();

							// write to the buffer 
							m_Camera.imageBuffer.push(actualCapturedFrame);
							break;
						}
					}

					// Release Memory from openCV
					
							
					
					
				//	is_RenderBitmap(cam_ID, m_lSeqMemId[j], m_hWndDisplay, IS_RENDER_FIT_TO_WINDOW);
				//}

				if (m_nDispModeSel == e_disp_mode_bitmap)
				{
					//is_RenderBitmap(cam_ID, cameraBuffer_Id, m_hWndDisplay, IS_RENDER_FIT_TO_WINDOW);
					// get the current image

					//currentFrame = nextImage();

				}
			}
		}
	}
}

void LocalVideo::getCurrentImage(capturedFrame &imageCamera)
{
	capturedFrame actualFrame;
	string myPath;
	string FileName("NoSignal.jpg");

	boost::filesystem::path p{ "/" };
	getFilePath(FileName, myPath);
	boost::filesystem::path pathToDefaultImage = boost::filesystem::path(myPath);

	/// thread loop
	//while (!stopCameraThread) {	

		actualFrame = m_Camera.imageBuffer.pop();		// get the last image from circular buffer
		imageCamera = actualFrame;
	
		cv::Mat defaultImage;
		std::string imageName;

		// check default image path

		//defaultImage = cv::imread(pathToDefaultImage.generic_string(), cv::IMREAD_COLOR);
		//defaultImage.copyTo(actualFrame.image);
		
		//cv::Mat image(cvSize(m_Camera.SensorInfo.nMaxWidth, m_Camera.SensorInfo.nMaxHeight), CV_8UC3, pPixelPointer);

}


// get the last available image
void LocalVideo::getImage(void){

	capturedFrame actualFrame;
	string myPath;
	string FileName("NoSignal.jpg");

	boost::filesystem::path p{ "/" };
	getFilePath(FileName, myPath);
	boost::filesystem::path pathToDefaultImage = boost::filesystem::path(myPath);

	/// thread loop
	while (!stopCameraThread) {	

		// read from the buffer using a lock guard mutex to keep a correct memory management
		actualFrame = m_Camera.imageBuffer.pop();

		//std::lock_guard<std::mutex> lock(bufferMutex);
		//bufferMutex.lock();
		//if (m_Camera.imageBuffer.size() > 0) {
			//actualFrame = m_Camera.imageBuffer.front();		// get the last image from circular buffer
			//imageCamera = actualFrame;
		//}
		//else {
			cv::Mat defaultImage;
			std::string imageName;

			// check default image path
	
			defaultImage = cv::imread(pathToDefaultImage.generic_string(), cv::IMREAD_COLOR);
			defaultImage.copyTo(actualFrame.image);

			//cv::Mat image(cvSize(m_Camera.SensorInfo.nMaxWidth, m_Camera.SensorInfo.nMaxHeight), CV_8UC3, pPixelPointer);
		}
		//bufferMutex.unlock();
	
}

// get the next image
cv::Mat LocalVideo::nextImage(void){

	INT nRet;
	int nChannels = 3;
	cv::Mat currentImage;	
	// Prepare OpenCV Image
	frame = cvCreateImageHeader(cvSize(m_Camera.SensorInfo.nMaxWidth, m_Camera.SensorInfo.nMaxHeight), IPL_DEPTH_8U, nChannels);
	void *pMemVoid;
	char* pMemChar;
	INT memoryID;

	// get Data from the uEye Memory
	is_GetImageMem(m_Camera.camID, &pMemVoid);
	is_GetActiveImageMem(m_Camera.camID, &pMemChar, &memoryID);
	char* pPixelPointer;
	pPixelPointer = reinterpret_cast<char*> (pMemVoid);

	// Build the Mat
	cv::Mat image(cvSize(m_Camera.SensorInfo.nMaxWidth, m_Camera.SensorInfo.nMaxHeight), CV_8UC3, pPixelPointer);

	// copy uEye data to IplImage
	cvSetData(frame, pPixelPointer, frame->widthStep);
	currentImage = cv::cvarrToMat(frame);

	return currentImage;
}

// set the frame rate for the camera
void LocalVideo::setFrameRate(double &fps){

	double newFps;
	frameRate = fps;
	is_SetFrameRate(m_Camera.camID, frameRate, &newFps);

}

// stop the camera 
void LocalVideo::stopCamera(){

	stopCameraThread = true;

}


// PRIVATE METHODS

INT LocalVideo::OpenCamera(HIDS hCamID){

	INT nRet;	
	bool isCamAvailable;
	cam_ID = hCamID;

	m_Camera.camID = hCamID;
	m_Camera.RenderMode = _disp_mode::e_disp_mode_bitmap;

	// Set the buffer
	//m_Camera.imageBuffer.set_capacity(DEFAULT_BUFFER_SIZE);

	// check camera availability
	isCamAvailable = checkCamera(hCamID);

	if (isCamAvailable) {

		nRet = InitCamera(&m_Camera.camID, m_hWndDisplay);
		if (nRet == IS_SUCCESS)
		{
			// Set open camera flag
			m_Camera.IsOpened = true;
			m_Camera.OnLive = false;

			// enable the dialog based error report
			nRet = is_SetErrorReport(m_Camera.camID, IS_ENABLE_ERR_REP); // or IS_DISABLE_ERR_REP);
			if (nRet != IS_SUCCESS)
			{
				//AfxMessageBox("ERROR: Can not enable the automatic uEye error report!", MB_ICONEXCLAMATION, 0);
				std::cout << "ERROR: Can not enable the automatic uEye error report!" << std::endl;
				return false;
			}

			// Retrieve sensor info
			is_GetSensorInfo(m_Camera.camID, &m_Camera.SensorInfo);

			GetMaxImageSize(&m_nSizeX, &m_nSizeY);

			if (m_Camera.SensorInfo.nColorMode == IS_COLORMODE_BAYER)
			{
				// for color camera models use RGB24 mode
				m_nColorMode = IS_CM_BGR8_PACKED;
				m_nBitsPerPixel = 24;
			}
			else if (m_Camera.SensorInfo.nColorMode == IS_COLORMODE_CBYCRY)
			{
				// for CBYCRY camera models use RGB32 mode
				m_nColorMode = IS_CM_BGRA8_PACKED;
				m_nBitsPerPixel = 32;
			}
			else
			{
				// for monochrome camera models use Y8 mode
				m_nColorMode = IS_CM_MONO8;
				m_nBitsPerPixel = 8;
			}

			// display initialization
			InitDisplayMode(m_Camera.RenderMode);

			// set exposure = 1/framerate
			double dExposure = 0.0;
			is_Exposure(m_Camera.camID, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&dExposure, sizeof(dExposure));


		}
		else {

			//AfxMessageBox("ERROR: Cannot open uEye camera!", MB_ICONEXCLAMATION, 0);
			//PostQuitMessage(0);
			std::cout << "ERROR: Cannot open uEye camera!" << std::endl;

		}
	}

	return true;
}

INT LocalVideo::InitCamera(HIDS *hCam, HWND hWnd){

	INT nRet = is_InitCamera(hCam, hWnd);
	/************************************************************************************************/
	/*                                                                                              */
	/*  If the camera returns with "IS_STARTER_FW_UPLOAD_NEEDED", an upload of a new firmware       */
	/*  is necessary. This upload can take several seconds. We recommend to check the required      */
	/*  time with the function is_GetDuration().                                                    */
	/*                                                                                              */
	/*  In this case, the camera can only be opened if the flag "IS_ALLOW_STARTER_FW_UPLOAD"        */
	/*  is "OR"-ed to hCam. This flag allows an automatic upload of the firmware.                 */
	/*                                                                                              */
	/************************************************************************************************/
	if (nRet == IS_STARTER_FW_UPLOAD_NEEDED)
	{
		// Time for the firmware upload = 25 seconds by default
		INT nUploadTime = 25000;
		is_GetDuration(*hCam, IS_STARTER_FW_UPLOAD, &nUploadTime);

		std::string Str1, Str2, Str3;
		Str1 = "This camera requires a new firmware. The upload will take about";
		Str2 = "seconds. Please wait ...";
		std::cout << Str1 << nUploadTime / 1000 << Str2 << std::endl;
		
		// Set mouse to hourglass
		//SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

		// Try again to open the camera. This time we allow the automatic upload of the firmware by
		// specifying "IS_ALLOW_STARTER_FIRMWARE_UPLOAD"
		*hCam = (HIDS)(((INT)*hCam) | IS_ALLOW_STARTER_FW_UPLOAD);
		nRet = is_InitCamera(hCam, hWnd);
	}

	return nRet;


	/////////////////////////////

	//int width = getWidth();
	//int height = getHeight();
	//int nChannels = 3;
	////m_nBitsPerPixel = 8;

	//// IplImage to save image from the uEye camera
	//frame = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, nChannels);

	//// Init uEye memory for the IDS driver
	//nRet = is_AllocImageMem(cam_ID, width, height, m_nBitsPerPixel, &cameraBuffer, &cameraBuffer_Id);

	//// active the memory to be used as a image frame buffer by the IDS driver
	//nRet = is_SetImageMem(cam_ID, cameraBuffer, cameraBuffer_Id);
	//
	//// setup IplImage
	//frame->ID = 0;
	//frame->nChannels = nChannels;
	//frame->alphaChannel = 0;
	//frame->depth = IPL_DEPTH_8U;
	//frame->dataOrder = 0;
	//frame->origin = 0;

	//frame->align = 1;
	//frame->width = width;
	//frame->height = height;
	//frame->roi = NULL;
	//frame->maskROI = NULL;
	//frame->imageId = NULL;
	//frame->tileInfo = NULL;
	//frame->imageSize = nChannels*width*height;
	//frame->widthStep = 4*width;

	//frame->imageDataOrigin = (char*)cameraBuffer;
	//frame->imageData = (char*)cameraBuffer;	

	//// Init frame events windows version
	//hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//nRet = is_InitEvent(cam_ID, hEvent, IS_SET_EVENT_FRAME);
	//nRet = is_EnableEvent(cam_ID, IS_DONT_WAIT);

	//return nRet;
}


// start the camera
void LocalVideo::StartCamera()
{
	is_CaptureVideo(m_Camera.camID, IS_WAIT);
}

bool LocalVideo::InitializeEvents()
{
	char str[128];
	bool bRet;

	// enumerate all events
	EventEnumerate();

	// init and run events
	for (int i = 0; i< m_MaxEvents; i++)
	{
		// create event
		m_hEventList[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hEventList == NULL)
		{
			sprintf(str, "ERROR in InitializeEvents() - CreateEvent #%d", i);
			//AfxMessageBox(str, MB_ICONEXCLAMATION, 0);
			return false;
		}

		// init event
		bRet = InitEvent( m_nEventType[i]);
		if (bRet == false)
		{
			sprintf(str, "ERROR in InitializeEvents() - InitEvent #%d", i);
			//AfxMessageBox(str, MB_ICONEXCLAMATION, 0);
			return false;
		}

		// enable event
		bRet = EnableEvent( m_nEventType[i], true);
		if (bRet == false)
		{
			sprintf(str, "ERROR in InitializeEvents() - EnableEvent #%d", i);
			//AfxMessageBox(str, MB_ICONEXCLAMATION, 0);
			return false;
		}

		// reset counter
		m_nEventCounter[i] = 0;
	}

	// create thread for events using C++11
	//std::function<void(LocalVideo)>uEyeCapturingFunction = &LocalVideo::ThreadProc;

	//std::thread uEyeThread(uEyeCapturingFunction, this);
	//uEyeThread.join();

	// create thread for events
	//m_hThread = (HANDLE)_beginthreadex(NULL, 0, threadProc, (void*)this, 0, (UINT*)&m_dwThreadID);
	//if (m_hThread == NULL)
	//{
	//	//AfxMessageBox("ERROR: Cannot create event tread!", MB_ICONEXCLAMATION, 0);
	//	return false;
	//}

	// we really shouldn't do that !!!!
	//SetThreadPriority(uEyeThread.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);

	// event handler thread must now be active
	m_boRunThread = TRUE;

	// GUI
	//OnButtonResetEvents();

	return true;
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
int  LocalVideo::InitDisplayMode(int modeSelection){

	INT nRet;

	if (m_Camera.camID == NULL)
	{
		return IS_NO_SUCCESS;
	}

	if (cameraBuffer != NULL)
	{
		is_FreeImageMem(m_Camera.camID, cameraBuffer, cameraBuffer_Id);
	}
	cameraBuffer = NULL;

	switch (modeSelection)
	{
	case e_disp_mode_direct3d:

		// if initializing the Direct3D mode succeeded then
		// leave the switch tree else run through to bitmap mode
		nRet = is_SetDisplayMode(m_Camera.camID, IS_SET_DM_DIRECT3D);
		if (nRet == IS_SUCCESS)
		{
			is_DirectRenderer(m_Camera.camID, DR_ENABLE_SCALING, NULL, NULL);

			// setup the color depth to the current VGA setting
			is_GetColorDepth(m_Camera.camID, &m_nBitsPerPixel, &m_nColorMode);
			break;
		}
		else
		{
			m_nDispModeSel = e_disp_mode_bitmap;
			//UpdateData(FALSE);
		}

	case e_disp_mode_bitmap:
	{
		nRet = is_SetDisplayMode(m_Camera.camID, IS_SET_DM_DIB);

		IS_SIZE_2D imageSize;
		is_AOI(m_Camera.camID, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

		INT nAllocSizeX = 0;
		INT nAllocSizeY = 0;

		m_nSizeX = nAllocSizeX = imageSize.s32Width;
		m_nSizeY = nAllocSizeY = imageSize.s32Height;

		UINT nAbsPosX = 0;
		UINT nAbsPosY = 0;

		// absolute pos?
		is_AOI(m_Camera.camID, IS_AOI_IMAGE_GET_POS_X_ABS, (void*)&nAbsPosX, sizeof(nAbsPosX));
		is_AOI(m_Camera.camID, IS_AOI_IMAGE_GET_POS_Y_ABS, (void*)&nAbsPosY, sizeof(nAbsPosY));

		if (nAbsPosX)
		{
			nAllocSizeX = m_sInfo.nMaxWidth;
		}
		if (nAbsPosY)
		{
			nAllocSizeY = m_sInfo.nMaxHeight;
		}
		// allocate an image memory.
		if (is_AllocImageMem(cam_ID,
			nAllocSizeX,
			nAllocSizeY,
			m_nBitsPerPixel,
			&cameraBuffer,
			&cameraBuffer_Id) != IS_SUCCESS)
		{
			//AfxMessageBox("Memory allocation failed!", MB_ICONWARNING);
			std::cout << "Memory allocation failed!" << std::endl;
		}
		else
		{
			is_SetImageMem(m_Camera.camID, cameraBuffer, cameraBuffer_Id);
			m_nDispModeSel = e_disp_mode_bitmap;
			//UpdateData(FALSE);// used for windows alerting for messaging
		}
	}
	break;
	}

	if (nRet == IS_SUCCESS)
	{
		// set the desired color mode
		is_SetColorMode(m_Camera.camID, m_nColorMode);

		// set the image size to capture
		IS_SIZE_2D imageSize;
		imageSize.s32Width = m_nSizeX;
		imageSize.s32Height = m_nSizeY;

		is_AOI(m_Camera.camID, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));
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

// Check if the camera is available to be used
bool LocalVideo::checkCamera(HIDS camID)
{
	bool is_Available = false;
	int nCameraCount;
	if (is_GetNumberOfCameras(&nCameraCount) == IS_SUCCESS) 
	{
		// allocate the required list size
		PUEYE_CAMERA_LIST pucl = (PUEYE_CAMERA_LIST)new char[sizeof(DWORD) + nCameraCount * sizeof(UEYE_CAMERA_INFO)];
		pucl->dwCount = nCameraCount;
		
		// receive the camera list
		if (is_GetCameraList(pucl) == IS_SUCCESS)
		{
			for (int i=0; i< nCameraCount; i++)
			{
				// Type = 0 -> USB, Type = 1 -> ETH
				int Type = (pucl->uci[i].dwDeviceID >= 1000);

				// If ETH camera, Network Mode (Not Used Here)
				
				// If USB camera
				if (Type == 0)
				{
					bool FirmwareDownloadSupported = (pucl->uci[i].dwStatus & FIRMWARE_DOWNLOAD_NOT_SUPPORTED) == 0;
					bool InterfaceSpeedSupported = (pucl->uci[i].dwStatus & INTERFACE_SPEED_NOT_SUPPORTED) == 0;

					// check if this camera ID is available
					if (pucl->uci[i].dwCameraID == camID)


					// If device is not used 
					if (pucl->uci[i].dwInUse == 0)
					{							
						// check if this camera ID is available
						if (pucl->uci[i].dwCameraID == camID)
						{
							is_Available = true;
							// save all data for this camera: Camera ID, Device ID, Model, Serial Number, etc
							m_Camera.uEyeInfo = pucl->uci[i];
						}
					}else{
						// is being used
						is_Available = false;
					}				
				}
			}
		}
	}
	return is_Available;
}

INT LocalVideo::getCameraID()
{
	return cam_ID;

}

bool LocalVideo::EventEnumerate() 
{
	// Initialize event types supported by uEye cameras
	m_nEventType[0] = IS_SET_EVENT_FRAME;
	m_nEventType[1] = IS_SET_EVENT_EXTTRIG;
	m_nEventType[2] = IS_SET_EVENT_VSYNC;
	m_nEventType[3] = IS_SET_EVENT_SEQ;
	m_nEventType[4] = IS_SET_EVENT_CAPTURE_STATUS;
	m_nEventType[5] = IS_SET_EVENT_DEVICE_RECONNECTED;
	m_nEventType[6] = IS_SET_EVENT_MEMORY_MODE_FINISH;
	m_nEventType[7] = IS_SET_EVENT_REMOVE;
	m_nEventType[8] = IS_SET_EVENT_REMOVAL;
	m_nEventType[9] = IS_SET_EVENT_NEW_DEVICE;
	m_nEventType[10] = IS_SET_EVENT_CONNECTIONSPEED_CHANGED;

	// enable IS FRAME Event
	m_nEventsChecked[0] = 2; 
	m_nEventsChecked[1] = 3;
	m_nEventsChecked[2] = 4;
	m_nEventsChecked[3] = 5;
	m_nEventsChecked[4] = 8;
	m_nEventsChecked[5] = 9;
	m_nEventsChecked[6] = 10;
	m_nEventsChecked[7] = 128;
	m_nEventsChecked[8] = 129;
	m_nEventsChecked[9] = 130;
	m_nEventsChecked[10] = 18;

	return true;
}

bool LocalVideo::GetIndexEvent(INT nEvent, int* pnIndex)
{
	// search for index
	for (int i = 0; i< m_MaxEvents; i++)
	{
		if (m_nEventsChecked[i] == nEvent)
		{
			*pnIndex = i;
			return true;
		}
	}

	// found no match
	//AfxMessageBox("ERROR in EvGetIndex()", MB_ICONEXCLAMATION, 0);
	std::cout << "ERROR in GetIndexEvent() index" << std::endl;
	return false;

}

bool LocalVideo::EnableEvent(INT nEvent, bool bEnable)
{
	if (bEnable == true)
	{
		// enable event
		m_Ret = is_EnableEvent(m_Camera.camID, nEvent);
		if (m_Ret != IS_SUCCESS)
		{
			return false;
		}
	}
	else
	{
		// enable event
		m_Ret = is_DisableEvent(m_Camera.camID, nEvent);
		if (m_Ret != IS_SUCCESS)
		{
			return false;
		}
	}

	return true;
}

bool LocalVideo::InitEvent(INT nEvent)
{
	int   nIndex;
	bool  bRet;

	// get event index
	bRet = GetIndexEvent(nEvent, &nIndex);
	if (bRet == false)
	{
		//AfxMessageBox("ERROR in EvInit() index", MB_ICONEXCLAMATION, 0);
		std::cout << "ERROR in InitEvent() index" << std::endl;
		return false;
	}

	// init event
	m_Ret = is_InitEvent(m_Camera.camID, m_hEventList[nIndex], nEvent);
	if (m_Ret != IS_SUCCESS)
	{
		return false;
	}

	return true;
}

void LocalVideo::ThreadProc()
{
	DWORD lReturn;
	int nEvIndex;
	DWORD static Time1, Time2;

	// run the event handler
	do
	{
		// wait to get signaled
		lReturn = WaitForMultipleObjects(m_MaxEvents, m_hEventList, FALSE, INFINITE);
		if (m_boRunThread)
		{
			nEvIndex = lReturn - WAIT_OBJECT_0;

			// get time stamp
			m_nEventsChecked[nEvIndex] = ::GetTickCount();

			// update counter and GUI
			m_nEventCounter[nEvIndex] ++;
			//SetDlgItemInt(IDC_EDIT_EV_0 + nEvIndex, m_nEvCount[nEvIndex]);

			//CheckDlgButton(IDC_RADIO_EVENT_0 + nEvIndex, BST_CHECKED);

			// display the sequenzbuffer or in bitmap live mode
			if (m_nEventType[nEvIndex] == IS_SET_EVENT_FRAME)
			{
				// in sequence mode
				/*if (!GetDlgItem(IDC_BUTTON_SEQ_START)->IsWindowEnabled()
					& !GetDlgItem(IDC_BUTTON_START_LIVE)->IsWindowEnabled()
					& GetDlgItem(IDC_BUTTON_SEQ_STOP)->IsWindowEnabled())*/
				{
					// find the latest image buffer and display the buffer
					INT nNum;
					char *pcMem, *pcMemLast;
					is_GetActSeqBuf(cam_ID, &nNum, &pcMem, &pcMemLast);
					int j;
					for (j = 0; j<SEQ_BUFFERS; j++)
					{
						if (pcMemLast == m_pcSeqImgMem[j])
						{
							break;
						}
					}

					is_RenderBitmap(cam_ID, m_lSeqMemId[j], m_hWndDisplay, IS_RENDER_FIT_TO_WINDOW);
				}

				if (m_nDispModeSel == e_disp_mode_bitmap)
				{
					is_RenderBitmap(cam_ID, cameraBuffer_Id, m_hWndDisplay, IS_RENDER_FIT_TO_WINDOW);
				}
			}
		}
	} while (m_boRunThread);
}


bool LocalVideo::TerminateAllEvents()
{
	// stop events in handler routine
	m_boRunThread = false;

	// launch an event to leave the thread loop
	SetEvent(m_hEventList[m_MaxEvents - 1]);
	if (WaitForSingleObject(m_hThread, 1000) != WAIT_OBJECT_0)
	{
		// finally terminate thread if it does not by itself
		TerminateThread(m_hThread, 0);
	}

	CloseHandle(m_hThread);
	m_hThread = NULL;

	// terminate and close events
	for (int i = 0; i< m_MaxEvents; i++)
	{
		CloseHandle(m_hEventList[i]);
		m_hEventList[i] = NULL;
	}

	return true;
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

// Build a uEye Sequence Memory (Ring Buffering)
bool LocalVideo::SeqBuilt()
{
	int i;

	// check if camera is already OnLive 
	if (m_Camera.OnLive)
	{
		SeqKill();
	}

	IS_SIZE_2D imageSize;
	is_AOI(m_Camera.camID, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

	INT nAllocSizeX = 0;
	INT nAllocSizeY = 0;

	m_nSizeX = nAllocSizeX = imageSize.s32Width;
	m_nSizeY = nAllocSizeY = imageSize.s32Height;

	UINT nAbsPosX = 0;
	UINT nAbsPosY = 0;

	// absolute pos?
	is_AOI(m_Camera.camID, IS_AOI_IMAGE_GET_POS_X_ABS, (void*)&nAbsPosX, sizeof(nAbsPosX));
	is_AOI(m_Camera.camID, IS_AOI_IMAGE_GET_POS_Y_ABS, (void*)&nAbsPosY, sizeof(nAbsPosY));

	if (nAbsPosX)
	{
		nAllocSizeX = m_Camera.SensorInfo.nMaxWidth;
	}
	if (nAbsPosY)
	{
		nAllocSizeY = m_Camera.SensorInfo.nMaxHeight;
	}

	// alloc seq buffer
	for (i = 0; i< SEQ_BUFFERS; i++)
	{
		// alloc memory
		m_Ret = is_AllocImageMem(m_Camera.camID,
			nAllocSizeX,
			nAllocSizeY,
			m_nBitsPerPixel,
			&m_pcSeqImgMem[i],
			&m_lSeqMemId[i]);
		if (m_Ret != IS_SUCCESS)
		{
			//AfxMessageBox("ERROR: cannot allocate sufficient sequence buffer memory", MB_ICONEXCLAMATION, 0);
			std::cout << "ERROR: cannot allocate sufficient sequence buffer memory" << std::endl;
			is_ExitCamera(m_Camera.camID);
			
		}
	}
	for (i = 0; i< SEQ_BUFFERS; i++)
	{
		// put memory into seq buffer
		m_Ret = is_AddToSequence(m_Camera.camID, m_pcSeqImgMem[i], m_lSeqMemId[i]);
		if (m_Ret != IS_SUCCESS)
		{
			std::cout << "ERROR: cannot put buffer into sequence" << std::endl;
			return false;
		}
	}
	return true;
}


// DESCRIPTION: Release the sequence for acquisition into the sequence buffer
bool LocalVideo::SeqKill()
{
	INT nRet;
	nRet = is_ClearSequence(m_Camera.camID);

	if (nRet == IS_SEQUENCE_LIST_EMPTY) 
	{
		return false;
	}

	if (nRet == IS_SUCCESS)
	{
		for (int i = 0; i < SEQ_BUFFERS; i++)
		{
			if ((m_pcSeqImgMem[i] != NULL) && (m_lSeqMemId[i] != 0))
			{
				// free memory
				m_Ret = is_FreeImageMem(m_Camera.camID, m_pcSeqImgMem[i], m_lSeqMemId[i]);
				if (m_Ret == IS_NO_SUCCESS || m_Ret == IS_SUCCESS)
				{
					m_pcSeqImgMem[i] = NULL;
					m_lSeqMemId[i] = 0;
				}
			}
		}
		return true;
	}
}

// Find the path for a given file
bool LocalVideo::getFilePath(string &fileName, string &pathFound) 
{
	bool found = false;

	// look for current path
	boost::filesystem::path directory;
	boost::filesystem::path currentPath = boost::filesystem::current_path();

	// get the number of elements of the path
	int pathElementsSize = 0;
	for (boost::filesystem::path::iterator it = currentPath.begin(); it != currentPath.end(); ++it) {
		pathElementsSize = pathElementsSize + 1;
	}

	// built the directory for search 2 levels up
	int levelUp = 2;
	boost::filesystem::path::iterator itToBuildPath = currentPath.begin();
	for (int i = 0; i < (pathElementsSize - levelUp); i++) {
		directory /= *itToBuildPath;
		++itToBuildPath;
	}

	boost::filesystem::path& path = directory;
	const boost::filesystem::path file = fileName;
	const boost::filesystem::recursive_directory_iterator end;
	const boost::filesystem::recursive_directory_iterator dir_iter(directory);

	const auto it = std::find_if(dir_iter,
		end,
		[&file](const boost::filesystem::directory_entry& e)
	{
		return e.path().filename() == file;
	});

	if (it != end) {

		path = it->path();
		pathFound = path.generic_string();		// make the path portable
		found = true;
	}
	return found;

}