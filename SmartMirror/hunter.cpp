#include "hunter.h"
#define ABS_TRESHOLD 15
#define PROP_TRESHOLD 0.3

void Hunter::initialize(String xmlPath, bool loadFromOpenCV)
{
	detector.initialize(xmlPath, loadFromOpenCV);
	tracker.initialize();
	initialPoints = 0;
}

bool Hunter::hunt(Mat * frame, Rect * outputRect)
{
	if ((tracker.getPointNum() < ABS_TRESHOLD) || (tracker.getPointNum() < PROP_TRESHOLD * initialPoints)) {
		/*detect*/
		if (!detector.detect(frame, outputRect)) {
			return false;
		}
		/*find good features to track*/
		tracker.initializeFeatures(frame, *outputRect);
		initialPoints = tracker.getPointNum();
	}
	else {
		/*track*/
		if (!tracker.track(frame)) {
			return false;
		}
	}
	
	return	true;
}
