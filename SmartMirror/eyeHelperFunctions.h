#ifndef EYE_HELPER_FUNCTIONS
#define EYE_HELPER_FUNCTIONS

#include <opencv2\core\core.hpp>
#include <boost\circular_buffer.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\video\video.hpp>
#include <iostream>
#include <vector>
#include "main.h"
#include <limits.h>

#define FIND_LAST 0
#define FIND_FIRST 1

Mat stretchHistogram(Mat *inputImage);
vector<int> sum(Mat *inputImage, int dim);
void findPeaks(vector<double>* input, vector<int>* locations,
	vector<double>* peaks, double minPeakHeight = -DBL_MAX);
void findPeaks(vector<int>* input, vector<int>* locations,
	vector<int>* peaks, int minPeakHeight = INT_MIN);
Mat binarize(Mat* inputImage, int threshold);
vector<double> meanValue(Mat *inputImage, int dim);
vector<double> diff(vector<double> * inputVector);
vector<int> diff(vector<int> * inputVector);
Mat invert(Mat* inputImage);
vector<int> getSortIndexes(vector<double> inputVector);
vector<int> ourFind(vector<int> vec01, int num = 0, int mode = FIND_FIRST);
vector<int> vectorTo01(vector<double> inputVector, double threshold);
vector<int> vectorTo01(vector<int> inputVector, double threshold);
vector<int> invert01(vector<int> inputVector);
void getDistance(vector<double> peaks, vector<int> locs, int * outputLoc1, int * outputLoc2);
void getBigInterval(vector<int> inputArray, int* outputBoundary1, int* outputBoundary2, double percentage = 0.75);
void vectorGetMax(vector<int> inputVector, int *outputMax, int *outputInd);
void vectorGetMin(vector<int> inputVector, int *outputMin, int *outputInd);
int eyelidDistance(Mat eye, double threshold);
vector<double> negateVector(vector<double> inputVector);
vector<int> negateVector(vector<int> inputVector);
void eyelidsDistances(Mat eyes, int* outputDL, int* outputDR, int leftMaximumOpened = 7, int rightMaximumOpened = 7);

typedef struct
{
	double PERCLOS;
	double MCD;
	double BF;
	double AOL;
	double OV;
	double CV;
} features_t;

features_t getFeatures(vector<int> leftEyeDistances, vector<int> rightEyeDistances);
#endif