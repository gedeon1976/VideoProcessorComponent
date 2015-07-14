#include "LocalVideo.h"


LocalVideo::LocalVideo()
{
	// set the size of the circular buffer
	imageBuffer.clear();
	//stopCameraThread = false;
	
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
void LocalVideo::setupCamera(int &cameraID, double width, double height){

	
	// setup the camera
	inputCapture.set(cv::CAP_PROP_FRAME_WIDTH, width);
	inputCapture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
	inputCapture.set(cv::CAP_PROP_FPS, frameRate);

	//// open the selected camera
	//localCamera.open(cameraID);
	//if (localCamera.isOpened()){

	//	// create a thread for reading the frames and write to the image buffer

	//	// store a call to the member function used for writing image data. 
	//	// This calling will be used as the thread callable function parameter with its arguments
	//	std::function<void(LocalVideo)> threadFunction = &LocalVideo::writeToBuffer;

	//	// use move constructor and start the thread
	//	cameraThread = std::thread(threadFunction, this);
	//			
	//	// wait for this thread!
	//	cameraThread.join();
	//}

}


// set the buffer size
void LocalVideo::setBufferSize(int bufferSize){

	imageBuffer.set_capacity(bufferSize);

}

// write to the buffer
void LocalVideo::writeToBuffer(void){

	//capturedFrame actualCapturedFrame;
	//cv::Mat currentFrame;
	//std::chrono::time_point<std::chrono::system_clock> currentCapturedTime;
	
	//// thread loop
	//while (!stopCameraThread){
	//
	//	// get the current image
	//	localCamera >> currentFrame;

	//	// save the captured frame
	//	currentFrame.copyTo(actualCapturedFrame.image);
	//	actualCapturedFrame.timeofCapture = std::chrono::system_clock::now();
	//	
	//	// write to the buffer using a lock guard mutex to keep a correct memory management
	//	bufferMutex.try_lock();
	//	imageBuffer.push_back(actualCapturedFrame);
	//	bufferMutex.lock();	

	//}
}

// get the last available image
void LocalVideo::getImage(cv::Mat &imageCamera){

	// read from the buffer using a lock guard mutex to keep a correct memory management
	//std::lock_guard<std::mutex> Guard(bufferMutex);
	//currentCapturedFrame = imageBuffer.front();		// get the last image from circular buffer
	//currentCapturedFrame.image.copyTo(imageCamera);
}

// get the next image
void LocalVideo::nextImage(void){

	capturedFrame currentCapturedFrame;
	std::chrono::time_point<std::chrono::system_clock> currentCapturedTime;

	if (inputCapture.isOpened())
	{
		cv::Mat currentImage;
		inputCapture >> currentImage;
		currentImage.copyTo(currentCapturedFrame.image);
		currentCapturedFrame.timeofCapture = std::chrono::system_clock::now();
		imageBuffer.push_back(currentCapturedFrame);
	}

}

// set the frame rate for the camera
void LocalVideo::setFrameRate(double &fps){

	frameRate = fps;

}

// stop the camera 
void LocalVideo::stopCamera(){

	//stopCameraThread = true;

}
