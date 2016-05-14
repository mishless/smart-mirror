#include "hunter.h"
#define ABS_TRESHOLD 15
#define PROP_TRESHOLD 0.3

bool Hunter::initialize(String xmlPath, bool loadFromOpenCV,
	                    unsigned int desiredWidth, unsigned int desiredHeight)
{
	if (!detector.initialize(xmlPath, loadFromOpenCV))
	{
		return false;
	}
	tracker.initialize();
	initialPoints = 0;

	width = desiredWidth;
	height = desiredHeight;

	return true;
}

bool Hunter::hunt(Mat * inputFrame, Mat* outputObject)
{
	Mat objectMask;
	Mat rotatedObject;
	Mat croppedObject;

	if ((tracker.getPointNum() < ABS_TRESHOLD) || (tracker.getPointNum() < PROP_TRESHOLD * initialPoints)) {
		/*detect*/
		if (!detector.detect(inputFrame, &initialRect)) {
			return false;
		}
		/*find good features to track*/
		tracker.initializeFeatures(inputFrame, initialRect);
		initialPoints = tracker.getPointNum();
		objectMask = Mat::zeros(inputFrame->size(), CV_8U);
		objectMask(initialRect) = 1;
		croppedObject = (*inputFrame)(initialRect);
	}
	else {
		/*track*/
		if (!tracker.track(inputFrame, &objectMask)) {			
			return false;
		}
		tracker.rotate(inputFrame, &objectMask, &rotatedObject);
		croppedObject = rotatedObject(initialRect);

	}

	/* This is very slow. If you need faster, remove resizing and just return
	   cropped object :( */
	resize(croppedObject, *outputObject, Size(width, height));
	return	true;
}
