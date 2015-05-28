#include "LocalVideo.h"


LocalVideo::LocalVideo()
{
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
void LocalVideo::startCamera(int &cameraID, int width, int height){

	
	// setup the camera


	// open the selected camera
	localCamera.open(cameraID);
	if (localCamera.isOpened){

		// create a thread for reading the frames and write to the image buffer

		// store a call to the member function used for writing image data. 
		// This calling will be used as the thread callable function parameter with its arguments
		std::function<void(LocalVideo)> threadFunction = &LocalVideo::writeToBuffer;

		std::thread capturingImageThread(threadFunction);
	}


}

// set the buffer size
void LocalVideo::setBufferSize(int bufferSize){


}

// write to the buffer
void LocalVideo::writeToBuffer(){

	capturedFrame actualCapturedFrame;
	cv::Mat currentFrame;
	std::chrono::time_point<std::chrono::system_clock> currentCapturedTime;
	
	// thread loop
	for (;;){
	
		// get the current image
		localCamera >> currentFrame;

		// save the captured frame
		currentFrame.copyTo(actualCapturedFrame.image);
		actualCapturedFrame.timeofCapture = std::chrono::system_clock::now();
		
		// write to the buffer

	}


}

// get the last available image
void LocalVideo::getImage(cv::Mat &imageCamera){


}

// set the frame rate for the camera
void LocalVideo::setFrameRate(int &fps){


}

// stop the camera 
void LocalVideo::stopCamera(){



}
