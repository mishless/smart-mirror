#ifndef PHASE_DETECTOR_H
#define PHASE_DETECTOR_H

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

class PhaseDetector
{
public:
	double detect(vector<Mat> *foreheads, vector<Mat> *palms, double freq);
};
#endif