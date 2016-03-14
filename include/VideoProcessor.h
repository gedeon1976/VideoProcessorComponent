
#include "InterfaceVideoProcessor.hpp"
#include "LocalVideo.h"

using namespace std;

class VideoProcessor: public InterfaceVideoProcessor{
	
public:
		/// create the interface implementing the virtual constructor
		InterfaceVideoProcessor *Create(){return new VideoProcessor();};
		~VideoProcessor(){};
		
		void Init(bool &init);
		
		void connectToRemoteCameras(std::string &leftCameraURL, std::string &rightCameraURL);	
		void connectToLocalCameras(int &leftCamera, int &rightCamera);	
		void getVideoImages(cv::Mat &leftImage, cv::Mat &rightImage);
		void calibrateCameras(bool calibrationFlag);	
	
private:
	
	int imageWidth, imageHeight;
	LocalVideo leftCameraVideo, rightCameraVideo;
	string leftCameraURL, rightCameraURL;
	cv::Mat leftImage, rightImage;

	
};