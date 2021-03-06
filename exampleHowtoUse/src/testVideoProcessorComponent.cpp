//This project test the VideoProcessor Component
//created with the purpose of getting connected to a
//pair of remote or local cameras
//at last used with uEye USB industrial cameras
		
#include "VideoProcessorAccess.h"
#include <opencv2\highgui.hpp>
		
using namespace std;
		
int main(int argc, char ** argv)
{
	// create the component using the factory method pattern
	VideoProcessorAccess AccessObject;
	InterfaceVideoProcessor *VideoProcessorComponent = AccessObject.CreateVideoProcessor();
		
	
	// Initialize component
	bool init = true;
	VideoProcessorComponent->Init(init);

	// call the methods from the VideoProcessor Component
	int leftID = 1;
	int rightID = 2;
	VideoProcessorComponent->connectToLocalCameras(leftID,rightID);
	cv::Mat leftImage, rightImage;

	// wait buffer filling
	cv::waitKey(250);
	
	cv::namedWindow("leftImage", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("rightImage", cv::WINDOW_AUTOSIZE);


		VideoProcessorComponent->getVideoImages(leftImage, rightImage);		
		//cv::imshow("leftImage", leftImage);
		//cv::imshow("rightImage", rightImage);
		//cv::waitKey(15);
	
	
	
	
	
	delete VideoProcessorComponent;
	return 0;
	
	}