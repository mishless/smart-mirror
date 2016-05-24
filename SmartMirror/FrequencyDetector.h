#ifndef __FREQUENCYDETECTOR_H_INCLUDED__
#define __FREQUENCYDETECTOR_H_INCLUDED__

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
#include "main.h"





using namespace std;
using namespace cv;

class FrequencyDetector {
private:
	double lowFrequency;
	double highFrequency;
	void filterSpectrum(vector<double> *inputSp, int *lowIndex, int *highIndex);
	void convertSpectrum(vector<double>* inputSp, vector<double>* outputSp);
public:
	void initialize(double lowFreq, double highFreq);
	double detectFrequency(vector<Mat>* frames);
};
#endif