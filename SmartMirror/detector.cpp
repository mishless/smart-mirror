#include "detector.h"

void Detector::initialize(String xmlPath, bool loadFromOpenCV) {
	// Initialize Boost library path
	String openCVLibraryPath;
	char* buf = 0;
	size_t sz = 0;
	if (_dupenv_s(&buf, &sz, "OPENCV_DIR") == 0)
	{
		openCVLibraryPath = (String)buf;
		free(buf);
	}
	// Initialize classifier
	if (loadFromOpenCV) {
		objectClassifier.load(openCVLibraryPath + xmlPath);
	}
	else {
		objectClassifier.load(xmlPath);
	}

	objectDetected = false;
	
//	faceCascade.load(boostLibraryPath + "/../../../sources/data/haarcascades/haarcascade_frontalface_alt2.xml");
	//eyesCascade.load(boostLibraryPath + "/../../../sources/data/haarcascades/haarcascade_mcs_eyepair_big.xml");
	//handsCascade.load("hands_final.xml"); 
}

bool Detector::detect(Mat* frame, Mat* mask) {
	// Intialize an empty mask
	(*mask) = Mat::zeros(frame->size(), CV_8U);

	// Detect faces
	objectClassifier.detectMultiScale((*frame), recognizedObjects, 1.1, 3);

	if (recognizedObjects.size() < 0) {
		return false;
	}
	// Draw circles on the detected faces
	for (int i = 0; i < recognizedObjects.size(); i++)
	{
		Point center(recognizedObjects[i].x, recognizedObjects[i].y);
		rectangle((*frame), Rect(recognizedObjects[0].x, recognizedObjects[0].y, recognizedObjects[0].width, recognizedObjects[0].height), Scalar(255, 0, 255));
	}
	(*mask)(Rect(recognizedObjects[0].x, recognizedObjects[0].y, recognizedObjects[0].width, recognizedObjects[0].height)) = 1;
	return true;
}

bool Detector::isDetected() {
	return objectDetected;
}

void Detector::setDetected(bool state) {
	objectDetected = state;
}

