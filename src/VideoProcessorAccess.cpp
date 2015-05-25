
#include "VideoProcessorAccess.h"

InterfaceVideoProcessor *VideoProcessorAccess::CreateVideoProcessor(){
	
	InterfaceVideoProcessor *Obj = new VideoProcessor();
	return Obj;
	
}
