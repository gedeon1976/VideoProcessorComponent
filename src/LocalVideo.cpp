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
