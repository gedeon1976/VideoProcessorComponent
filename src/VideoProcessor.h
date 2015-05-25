
#include "InterfaceVideoProcessor.h"


class VideoProcessor: public InterfaceVideoProcessor{
	
public:
		/// create the interface implementing the virtual constructor
		InterfaceVideoProcessor *Create{return new VideoProcessor();};
		~VideoProcessor(){};
		
		void Init(bool &init);
		
		void connectToRemoteCameras(string &leftCameraURL, string &rightCameraURL);	
		void connectToLocalCameras(string &leftCamera, string &rightCamera);	
		void getVideoImages(cv::Mat &leftImage, cv::Mat &rightImage);
		void calibrateCameras(bool calibrationFlag);	
	
private:
	
		string leftCamera, rightCamera;
		cv::Mat leftImage, rightImage;
	
};