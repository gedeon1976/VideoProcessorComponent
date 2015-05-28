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
