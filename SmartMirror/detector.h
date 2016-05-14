#ifndef __DETECTOR_H_INCLUDED__
#define __DETECTOR_H_INCLUDED__

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

using namespace std;
using namespace cv;

class Detector {
private:
	vector<Rect> recognizedObjects;
	CascadeClassifier objectClassifier;
public:
	bool initialize(String xmlPath, bool loadFromOpenCV);
	bool detect(Mat* frame, Rect* faceRect);
};
#endif