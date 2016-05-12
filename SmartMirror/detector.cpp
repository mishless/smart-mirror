#include "detector.h"

void Detector::initialize() {
	// Initialize Boost library path
	String boostLibraryPath;
	char* buf = 0;
	size_t sz = 0;
	if (_dupenv_s(&buf, &sz, "OPENCV_DIR") == 0)
	{
		boostLibraryPath = (String)buf;
		free(buf);
	}
	// Initialize classifier
	faceCascade.load(boostLibraryPath + "/../../../sources/data/haarcascades/haarcascade_frontalface_alt2.xml");
	// Initialize eyesCascade and handsCascade - everyone can do it! :)
}

bool Detector::detectFace(Mat* frame, Mat* mask) {
	// Intialize an empty mask
	(*mask) = Mat::zeros(frame->size(), CV_8U);

	// Detect faces
	faceCascade.detectMultiScale((*frame), faces, 1.1, 3);

	if (faces.size() < 0) {
		return false;
	}
	// Draw circles on the detected faces
	for (int i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x, faces[i].y);
		rectangle((*frame), Rect(faces[0].x, faces[0].y, faces[0].width, faces[0].height), Scalar(255, 0, 255));
	}
	(*mask)(Rect(faces[0].x, faces[0].y, faces[0].width, faces[0].height)) = 1;
	return true;
}