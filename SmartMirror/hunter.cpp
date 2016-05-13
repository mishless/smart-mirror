#include "hunter.h"

void Hunter::initialize(String xmlPath, bool loadFromOpenCV)
{
	detector.initialize(xmlPath, loadFromOpenCV);
	tracker.initialize();
}

bool Hunter::hunt(Mat * frame, Rect * outputRect)
{
	if(tracker.getPointNum() < ABS_TRESHOLD || tracker.getPointNum() < PROP_TRESHOLD * 
	return false;
}
