#ifndef __HUNTER_H_INCLUDED__
#define __HUNTER_H_INCLUDED__

#include <opencv2\core\core.hpp>
#include <boost\circular_buffer.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\video\video.hpp>
#include <iostream>
#include <vector>
#include "frame.h"
#include "detector.h"
#include "tracker.h"

class Hunter {
private:
	unsigned int width;
	unsigned int height;
	Detector detector;
	Tracker tracker;
	size_t initialPoints;
	Rect initialRect;
public:
	bool initialize(String xmlPath, bool loadFromOpenCV,
		            unsigned int desiredWidth, unsigned int desiredHeight);
	bool hunt(Mat* frame, Mat* outputMask);
};


#endif