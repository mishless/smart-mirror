#ifndef __TRACKER_H_INCLUDED__
#define __TRACKER_H_INCLUDED__

#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\video\video.hpp>
#include <vector>

using namespace cv;
using namespace std;

class Tracker {
private:
	Mat initialMask;
	Mat rigidTransform;
	Mat previousGreyFrame;
	vector<Point2f> previousPoints;
public:
	void initialize();
	bool track(Mat* frame, Mat* outputMask);
	void initializeFeatures(Mat* object, Rect faceRect);
	size_t getPointNum();
	void rotate(Mat* inputFrame, Mat* inputMask, Mat* outputFrame);
};

const double QUALITY_LEVEL = 0.01;
const double MIN_DISTANCE = 10;
const int BLOCK_SIZE = 3;
const bool USE_HARRIS_DETECTOR = false;
const double K = 0.04;

#endif
