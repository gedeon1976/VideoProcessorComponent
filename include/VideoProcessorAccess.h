/*!  \brief
This class is the access door to the videoProcessor interface
following the factory method pattern design for the API access,
the next lines shows an example of how to use this component
to get access to its functionalities
\code{.cpp}
		//This project test the VideoProcessor Component
		//created for the purpose of get connected to a
		//pair of remote or local cameras
		
		#include "VideoProcessorAccess.h"
		
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
		VideoProcessor->startConnection();
		VideoProcessor->setGravityCompensation(enableGravity)
		while(1){
			// get the haptic position
			HapticComponent->getHapticPosition(hapticPosition);
			position = hapticPosition.getTraslation();
			cout << "X: " << position[0] << "Y: " << position[1] << "Z: " << position[2] << "\n" << endl;			
		}
		
		delete VideoProcessorComponent;
		return 0;
		}
\endcode
*/

#include "VideoProcessor.h"			// Interface API

class VideoProcessorAccess{
	public:
	/// Create the VideoProcessor component calling this factory method
	InterfaceVideoProcessor *CreateVideoProcessor();
	
};

