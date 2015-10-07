//This project test the VideoProcessor Component
//created with the purpose of getting connected to a
//pair of remote or local cameras
		
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
	int leftID = 0;
	int rightID = 1;
	VideoProcessorComponent->connectToLocalCameras(leftID,rightID);
	cv::Mat leftImage, rightImage;
	

	while (1)
	{
		VideoProcessorComponent->getVideoImages(leftImage, rightImage);
		cv::imshow("leftImage", leftImage);
		cv::imshow("rightImage", rightImage);
	}
	
	
	
	
	delete VideoProcessorComponent;
	return 0;
	
	}