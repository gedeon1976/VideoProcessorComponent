/* \brief

This is the Interface for the videoProcessor component that
serve as the source of the video images from the remote 
teleoperated environment!!

This is the Abstract Base Class that defines the interface for
this component

The API design uses the factory method software pattern
see : Chapter 3 Patterns of Book API C++ Design by M. Reedy 2011

some features are:

1. use of virtual constructors
2. use a factory method to enable the creation of the API class using a derived class
*/

/*
PROJECT:	3DPOINTER
COMPONENT:	VIDEO PROCESSOR
DATE:		25.05.15
AUTHOR:		HENRY PORTILLA
SPECIAL
THANKS:		GOD

*/
#pragma once
#include "commonVideoProcessorComponent.h"

class InterfaceVideoProcessor{

public:

	/// Not default constructors instead use of virtual constructor
	virtual InterfaceVideoProcessor* Create() = 0;
	virtual ~InterfaceVideoProcessor(){};
	
	/// initializes the component
	virtual void Init(bool &init) = 0;

	/// Connect to the remote cameras
	/// @param[in,out] leftCameraURL string that saves the remote address for the left camera
	/// @param[in,out] rightCameraURL string that saves the remote address for the right camera
	virtual void connectToRemoteCameras(string &leftCameraURL, string &rightCameraURL) = 0;
	
	/// Connect to the local cameras
	/// @param[in,out] leftCamera string that saves the local address for the left camera
	/// @param[in,out] rightCamera string that saves the local address for the right camera
	virtual void connectToLocalCameras(int &leftCamera, int &rightCamera) = 0;
	
	/// Get the video images from the cameras
	/// @param[in,out] leftCameraURL string that saves the local address for the left camera
	/// @param[in,out] rightCameraURL string that saves the local address for the right camera
	virtual void getVideoImages(cv::Mat &leftImage, cv::Mat &rightImage) = 0;
	
	/// Calibrate cameras
	/// @param[in,out] calibrationFlag enable or disable camera calibration
	virtual void calibrateCameras(bool calibrationFlag) = 0;



};