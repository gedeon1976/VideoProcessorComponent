
#include "VideoProcessor.h"

////////////////////////////////////////////////////////////////////////////
// interface realization (implementation) for the API of this component
////////////////////////////////////////////////////////////////////////////

// initialize the component
void VideoProcessor::Init(bool &Init){
	
	
}

// connect to the remote cameras
void VideoProcessor::connectToRemoteCameras(string &leftCameraURL, string &rightCameraURL){
	
	
}

// connect to the local cameras
void VideoProcessor::connectToLocalCameras(int &leftCamera, int &rightCamera){
	
	double desiredFps = 25;
	int bufferSize = 10;		//
	string leftName("LeftCamera");
	string rightName("RightCamera");

	// setup cameras
	leftCameraVideo.setCameraName(leftName);
	leftCameraVideo.setBufferSize(bufferSize);
	leftCameraVideo.setFrameRate(desiredFps);

	rightCameraVideo.setCameraName(rightName);
	rightCameraVideo.setBufferSize(bufferSize);
	rightCameraVideo.setFrameRate(desiredFps);

	// start threads for cameras images capturing
	leftCameraVideo.startCamera(leftCamera, imageWidth, imageHeight);
	rightCameraVideo.startCamera(rightCamera,imageWidth,imageHeight);	
	
}

// get the video images from the cameras
void VideoProcessor::getVideoImages(cv::Mat &leftImage, cv::Mat &rightImage){
	
}

// calibrate the cameras or not
void VideoProcessor::calibrateCameras(bool calibrationFlag){
	
	
}




////////////////////////////////////////////////////////////////////////////
// internal methods implementation
////////////////////////////////////////////////////////////////////////////