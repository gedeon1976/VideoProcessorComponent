
#include "VideoProcessor.h"

////////////////////////////////////////////////////////////////////////////
// interface realization (implementation) for the API of this component
////////////////////////////////////////////////////////////////////////////

// initialize the component
void VideoProcessor::Init(bool &Init){
	
	
}

// connect to the remote cameras
void VideoProcessor::connectToRemoteCameras(std::string &leftCameraURL, std::string &rightCameraURL){
	
	
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

	// setup the cameras
	leftCameraVideo.setupCamera(leftCamera, imageWidth, imageHeight);
	rightCameraVideo.setupCamera(rightCamera,imageWidth,imageHeight);	

	// start threads for cameras images capturing
	// assign the corresponding function for the threads
	std::function<void(LocalVideo)>threadCapturingFunction = &LocalVideo::writeToBuffer;

	std::thread leftVideoStream(threadCapturingFunction,leftCameraVideo);
	std::thread rightVideoStream(threadCapturingFunction,rightCameraVideo);
	
	leftVideoStream.join();
	rightVideoStream.join();
}

// get the video images from the cameras
void VideoProcessor::getVideoImages(cv::Mat &leftImage, cv::Mat &rightImage){

	capturedFrame leftFrame, rightFrame;
	leftCameraVideo.getImage(leftFrame);
	rightCameraVideo.getImage(rightFrame);

	leftFrame.image.copyTo(leftImage);
	rightFrame.image.copyTo(rightImage);
	
}

// calibrate the cameras or not
void VideoProcessor::calibrateCameras(bool calibrationFlag){
	
	
}




////////////////////////////////////////////////////////////////////////////
// internal methods implementation
////////////////////////////////////////////////////////////////////////////