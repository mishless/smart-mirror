#ifndef __RECOGNIZER_H_INCLUDED__
#define __RECOGNIZER_H_INCLUDED__

#include <string>
#include <vector>
#include "opencv2/contrib/contrib.hpp"
#include <opencv2\core\core.hpp>
#include <boost\circular_buffer.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\video\video.hpp>
#include <iostream>

using namespace std;
using namespace cv;

enum Gender {
	MALE,
	FEMALE,
	UNKNOWN
};

typedef struct
{
	string fullName;
	unsigned int height;
	unsigned int weight;
	Gender gender;
	int yearOfBirth;
} PersonInfo;


class Recognizer {
private:
	Ptr<FaceRecognizer> model;
	vector<PersonInfo> database;
public:
	void initialize(double threshold);
	bool train(string fileInfo, string databaseFolder);
	bool recognize(Mat image, PersonInfo *outputPersonInfo);
};






#endif